#include "import.hpp"
#include "varjus_api/internal/globalEnums.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"
#include "linter/linter.hpp"
#include "linter/tokenizer.hpp"
#include "linter/modules/module.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/exports/export.hpp"

#include "fs/fs_globals.hpp"
#include "fs/fs_io.hpp"

#include <cassert>
#include <format>

CImportLinter::CImportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {}

Success CImportLinter::Parse()
{

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_import) {

		m_pOwner->GetModule()->PushError( VSL("expected \"import\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (!ParseIdentifierRecursively())
		return failure;

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_from) {
		m_pOwner->GetModule()->PushError(VSL("expected \"from\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); // skip from

	if (!ParseFilePath())
		return failure;

	if (!ParseFile())
		return failure;

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_semicolon)) {
		m_pOwner->GetModule()->PushError(VSL("expected \";\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}


	return success;
}

Success CImportLinter::ParseIdentifierRecursively()
{

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		m_pOwner->GetModule()->PushError(VSL("expected a name"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (m_pOwner->GetContext()->m_sFilePath.empty()) {
		m_pOwner->GetModule()->PushError(VSL("imports are not supported when there is no working directory"),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	auto scope = m_pScope.lock();

	m_oNames.push_back((*m_iterPos)->Source());
	std::advance(m_iterPos, 1);
	
	if (IsEndOfBuffer() || (*m_iterPos)->Type() == tt_from || !(*m_iterPos)->IsOperator(p_comma)) {
		return success;
	}

	std::advance(m_iterPos, 1); // skip ,
	return ParseIdentifierRecursively();
}

Success CImportLinter::ParseFilePath()
{
	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_string) {
		m_pOwner->GetModule()->PushError(VSL("expected a string"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	const auto& relativePath = (*m_iterPos)->Source();
	const auto& wd = fs::previous_directory( m_pOwner->GetContext()->m_sFilePath );

	const auto fullPath = wd + DIRECTORY_SEPARATOR_CHAR + relativePath;

	if (!fs::file_exists(fullPath)) {
		m_pOwner->GetModule()->PushError(fmt::format(VSL("\"{}\" does not exist"), fullPath), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oTargetFile = fullPath;

	if (m_oTargetFile == m_pOwner->GetModule()->GetFilePath()) {
		m_pOwner->GetModule()->PushError(fmt::format(VSL("attempted to import a symbol that is exported in the same file"), fullPath),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}


	std::advance(m_iterPos, 1); // skip file name
	return success;
}

Success CImportLinter::ParseFile()
{
	auto modules = m_pOwner->GetProgramInformation()->GetModules();
	assert(modules);

	const auto& sourceFile = m_pOwner->GetContext()->m_sFilePath;
	if (!modules->m_oDependencyGraph.contains(sourceFile)) {
		modules->m_oDependencyGraph[sourceFile] = {};
	}

	if (modules->m_oVisitedModules.contains(m_oTargetFile)) {
		CModule* thisModule = modules->FindCachedModule(m_oTargetFile);

		if (!thisModule) { // means this file hasn't been parsed yet
			if (auto str = modules->CheckCircularDependencies(sourceFile, modules->m_oDependencyGraph)) {
				m_pOwner->GetModule()->PushError(*str);
				return failure;
			}
		}
	} else {
		modules->m_oVisitedModules.insert(m_oTargetFile);
	}

	modules->m_oDependencyGraph[sourceFile].push_back(m_oTargetFile);
	if (auto str = modules->CheckCircularDependencies(sourceFile, modules->m_oDependencyGraph)) {
		m_pOwner->GetModule()->PushError(*str);
		return failure;
	}

	CModule* thisModule = GetFileModule();

	if (!thisModule)
		return failure;

	for (auto& name : m_oNames) {
		const auto exportedSymbol = thisModule->GetExport(name);

		if (!exportedSymbol) {
			thisModule->PushError(fmt::format(VSL("\"{}\" is not an exported symbol"), name), GetIteratorSafe()->m_oSourcePosition);
			return failure;
		}
		if (exportedSymbol->Type() == es_variable) {
			if (!DeclareVariable(name, exportedSymbol, thisModule->GetIndex()))
				return failure;

			continue;
		} if (exportedSymbol->Type() == es_function) {
			if (!DeclareFunction(name, exportedSymbol, thisModule->GetIndex()))
				return failure;

			continue;
		}

		assert(false);
	}


	return success;

}
CModule* CImportLinter::GetFileModule() const
{
	const auto pinfo = m_pOwner->GetProgramInformation();
	assert(pinfo);
	auto modules = pinfo->GetModules();
	assert(modules);

	if (auto cachedModule = modules->FindCachedModule(m_oTargetFile))
		return cachedModule;
	
	auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(pinfo, m_oTargetFile, e_auto);
	auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
	auto begin = tokens.begin();
	auto end = tokens.end();

	CBufferLinter l(m_pOwner->GetProgramInformation(), begin, end, m_oTargetFile);

	if (!l.Parse())
		return nullptr;


	return l.GetModule(); // this is fine because the module is owned by CModule::m_oAllModules


}

Success CImportLinter::DeclareVariable(const VarjusString& symbolName,
	CExportedSymbol* const s, std::size_t moduleIndex)
{

	auto scope = m_pScope.lock();

	if (!scope) {
		m_pOwner->GetModule()->PushError(VSL("CImportLinter: internal error"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (!scope->DeclareVariable(symbolName)) {
		m_pOwner->GetModule()->PushError(fmt::format(VSL("variable \"{}\" already declared"), symbolName), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	auto var = m_pOwner->m_VariableManager->DeclareVariable(symbolName);
	var->m_bBelongsToDifferentModule = true;
	var->m_uModuleIndex = moduleIndex;
	var->m_uIndex = s->GetIndex();

	return success;
}

Success CImportLinter::DeclareFunction(const VarjusString& symbolName,
	CExportedSymbol* const s, std::size_t moduleIndex)
{

	if (m_pOwner->m_FunctionManager->ContainsFunction(symbolName)) {
		m_pOwner->GetModule()->PushError(fmt::format(VSL("function \"{}\" already declared"), symbolName), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	auto func = m_pOwner->m_FunctionManager->DeclareFunction(symbolName, m_pOwner->GetModule()->GetFunctionCount());

	func->m_bBelongsToDifferentModule = true;
	func->m_uModuleIndex = moduleIndex;
	func->m_uIndex = s->GetIndex();

	return success;
}
