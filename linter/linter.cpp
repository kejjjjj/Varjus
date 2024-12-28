#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"
#include "context.hpp"

#include "hoisting/hoisting.hpp"
#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "functions/stack.hpp"
#include "functions/function.hpp"
#include "scopes/scope.hpp"
#include "modules/imports/import.hpp"
#include "modules/exports/export.hpp"
#include "modules/module.hpp"
#include "statements/for/for.hpp"
#include "statements/while/while.hpp"
#include "statements/if/if.hpp"
#include "statements/if/else.hpp"
#include "statements/return/return.hpp"
#include "statements/control/control.hpp"
#include "statements/try_catch/try_catch.hpp"
#include "statements/throw/throw.hpp"

#include <cassert>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end, const std::string& filePath)
	: CLinter(start, end), m_oInitialPosition(start), m_sFilePath(filePath) {}
CFileLinter::~CFileLinter() = default;

static Success AddInstruction(LinterIterator& pos, RuntimeBlock&& block, const WeakScope& scope)
{
	if (block) { //nullptr when no initializer

		if (auto s = scope.lock()) {
			s->AddInstruction(std::move(block));
		} else {
			CLinterErrors::PushError("!(auto s = scope.lock())", (*pos)->m_oSourcePosition);
			return failure;
		}
	}

	return success;
}

template<typename Linter> Success Lint(const CLinterContext& ctx)
{

	Linter linter(ctx.m_iterPos, ctx.m_iterEnd, ctx.scope, ctx.memory);
	if (!linter.Parse())
		return failure;

	if constexpr (
		std::is_same_v<CFunctionLinter, Linter> || 
		std::is_same_v<CElseStatementLinter, Linter> ||
		std::is_same_v<CImportLinter, Linter>)
		return success;

	else if(ctx.m_bAddInstructions)
		return AddInstruction(ctx.m_iterPos, linter.ToRuntimeObject(), ctx.scope);
	else
		return success;
}

Success CFileLinter::LintOperator(const CLinterContext& ctx)
{
	// a new scope
	if ((*ctx.m_iterPos)->IsOperator(p_curlybracket_open)) {
		return LintScope(ctx);
	}

	// otherwise a normal expression
	return Lint<CLinterExpression>(ctx);
}
Success CFileLinter::LintScope(const CLinterContext& ctx)
{

	if (ctx.memory->IsGlobalMemory()) {
		CLinterErrors::PushError("unnamed scopes are not allowed in the global scope", (*ctx.m_iterPos)->m_oSourcePosition);
		return failure;
	}
	auto s = ctx.scope.lock();
	if (!s) {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", (*ctx.m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::shared_ptr<CScope> thisScope = s->CreateScope();

	CScopeLinter scopeLinter(ctx.m_iterPos, ctx.m_iterEnd, thisScope, ctx.memory);
	if (!scopeLinter.Parse())
		return failure;

	s->AddInstructions(thisScope->MoveInstructions());
	return success;

}
Success CFileLinter::LintFunctionAmbiguity(const CLinterContext& ctx)
{
	// a new function
	if(!ctx.memory->IsStack())
		return Lint<CFunctionLinter>(ctx);

	// otherwise a lambda
	return Lint<CLinterExpression>(ctx);
}
Success CFileLinter::LintToken(const CLinterContext& ctx)
{
	if (ctx.m_iterPos == ctx.m_iterEnd)
		return failure;

	switch ((*ctx.m_iterPos)->Type()) {
	case tt_let:
	case tt_const:
		return Lint<CVariableDeclarationLinter>(ctx);
	case tt_undefined:
	case tt_false:
	case tt_true:
	case tt_int:
	case tt_double:
	case tt_string:
	case tt_name:
	case tt_typeof:
	case tt_tostring:
		return Lint<CLinterExpression>(ctx);
	case tt_operator:
		return LintOperator(ctx);
	case tt_fn:
		return LintFunctionAmbiguity(ctx);
	case tt_if:
		return Lint<CIfStatementLinter>(ctx);
	case tt_else:
		return Lint<CElseStatementLinter>(ctx);
	case tt_for:
		return Lint<CForStatementLinter>(ctx);
	case tt_while:
		return Lint<CWhileStatementLinter>(ctx);
	case tt_return:
		return Lint<CReturnStatementLinter>(ctx);
	case tt_break:
	case tt_continue:
		return Lint<CLoopControlStatement>(ctx);
	case tt_try:
		return Lint<CTryCatchStatementLinter>(ctx);
	case tt_throw:
		return Lint<CThrowStatementLinter>(ctx);
	case tt_import:
		return Lint<CImportLinter>(ctx);
	case tt_export:
		return Lint<CExportLinter>(ctx);
	case tt_catch:
	case tt_error:
	case tt_from:
	case tt_unused_count:
	default:
		CLinterErrors::PushError("unexpected token: " + (*ctx.m_iterPos)->Source(), (*ctx.m_iterPos)->m_oSourcePosition);
		return failure;
	}

}


Success CFileLinter::ParseFile()
{
	if (!HoistFile())
		return failure;

	m_iterPos = m_oInitialPosition;

	return LintFile();
}

static void DeclareGlobalObjects(CMemory* m_pOwner, CScope* const scope)
{
	VectorOf<std::string> keys = { "console", "math" };

	for (auto& key : keys) {
		m_pOwner->m_VariableManager->DeclareVariable(key);
		if (!scope->DeclareVariable(key))
			return CLinterErrors::PushError("internal bug in DeclareGlobalObjects", {});
	}
}

Success CFileLinter::HoistFile()
{
	m_pHoister = std::make_unique<CHoister>();

	// no need to give a module index to hoisted files
	auto mod = std::make_unique<CModule>(m_sFilePath);

	CMemory globalMemory(nullptr, mod.get());
	auto globalScope = std::make_shared<CScope>(&globalMemory);

	DeclareGlobalObjects(&globalMemory, globalScope.get());

	CLinterContext ctx{
		.m_iterPos = m_iterPos,
		.m_iterEnd = m_iterEnd,
		.scope = globalScope,
		.memory = &globalMemory,
		.m_bAddInstructions = false
	};

	while (!IsEndOfBuffer()) {
		if (!LintToken(ctx))
			break;

		if (IsEndOfBuffer())
			break;

		std::advance(m_iterPos, 1);
	}

	for (auto& [name, value] : globalMemory.m_FunctionManager->GetIterator()) {
		m_pHoister->DeclareFunction(name, *value);
	}

	return success;
}

Success CFileLinter::LintFile()
{
	m_pModule = CModule::CreateNewModule(m_sFilePath);
	CMemory globalMemory(nullptr, m_pModule);
	globalMemory.m_pHoister = m_pHoister.get();

	auto globalScope = std::make_shared<CScope>(&globalMemory);

	DeclareGlobalObjects(&globalMemory, globalScope.get());

	CLinterContext ctx{
		.m_iterPos = m_iterPos,
		.m_iterEnd = m_iterEnd,
		.scope = globalScope,
		.memory = &globalMemory,
		.m_bAddInstructions = true
	};

	while (!IsEndOfBuffer()) {
		if (!LintToken(ctx))
			break;

		if (IsEndOfBuffer())
			break;

		std::advance(m_iterPos, 1);
	}

	m_pModule->AddGlobalInstructions(globalScope->MoveInstructions());
	m_pModule->SetGlobalVariableCount(globalMemory.m_VariableManager->GetVariableCount());

	return success;
}

