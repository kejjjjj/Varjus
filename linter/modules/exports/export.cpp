#include "export.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/functions/stack.hpp"
#include "linter/modules/module.hpp"
#include "linter/functions/function.hpp"

#include "linter/declarations/variable_declarations.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

using namespace Varjus;

CExportLinter::CExportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {
}
CExportLinter::~CExportLinter() = default;


Varjus::Success CExportLinter::Parse()
{

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_export) {
		m_pOwner->GetModule()->PushError(VSL("expected \"tt_export\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if(!m_pOwner->IsGlobalMemory())
		m_pOwner->GetModule()->PushError(VSL("exports are only allowed in the global scope"), GetIteratorSafe()->m_oSourcePosition);

	std::advance(m_iterPos, 1); // skip export

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected a declaration"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (CVariableDeclarationLinter::IsDeclaration(*m_iterPos)) {
		return ParseVariableDeclaration();
	} else if ((*m_iterPos)->Type() == tt_fn) {
		return ParseFunctionDeclaration();
	}

	m_pOwner->GetModule()->PushError(VSL("expected a declaration"), GetIteratorSafe()->m_oSourcePosition);
	return failure;
}
Varjus::Success CExportLinter::ParseVariableDeclaration()
{
	m_pVariableLinter = std::make_unique<CVariableDeclarationLinter>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!m_pVariableLinter->Parse())
		return failure;

	if (m_pOwner->IsHoisting()) // no action required
		return success; 

	const auto var = m_pVariableLinter->GetIdentifier();
	m_pOwner->GetModule()->AddExport(var->m_sName, std::make_unique<CExportedVariable>(var->m_uIndex));
	return success;
}
Varjus::Success CExportLinter::ParseFunctionDeclaration()
{
	auto fnLinter = std::make_unique<CFunctionLinter>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!fnLinter->Parse())
		return failure;

	if (m_pOwner->IsHoisting()) // no action required
		return success;

	auto func = m_pOwner->m_FunctionManager->GetFunction(fnLinter->GetName());
	m_pOwner->GetModule()->AddExport(fnLinter->GetName(), std::make_unique<CExportedFunction>(func->m_uIndex));
	return success;

}
__RuntimeBlock CExportLinter::ToRuntimeObject() const
{ 
	if (m_pVariableLinter) 
		return m_pVariableLinter->ToRuntimeObject();

	return nullptr;
}
