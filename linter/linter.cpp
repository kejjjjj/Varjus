#include "context.hpp"
#include "error.hpp"
#include "varjus_api/internal/globalEnums.hpp"
#include "linter.hpp"
#include "token.hpp"

#include "declarations/variable_declarations.hpp"
#include "expressions/expression.hpp"
#include "functions/function.hpp"
#include "functions/stack.hpp"
#include "hoisting/hoisting.hpp"
#include "modules/exports/export.hpp"
#include "modules/imports/import.hpp"
#include "modules/module.hpp"
#include "scopes/scope.hpp"
#include "statements/control/control.hpp"
#include "statements/for/for.hpp"
#include "statements/if/else.hpp"
#include "statements/if/if.hpp"
#include "statements/repeat/repeat.hpp"
#include "statements/return/return.hpp"
#include "statements/throw/throw.hpp"
#include "statements/try_catch/try_catch.hpp"
#include "statements/while/while.hpp"

#include "varjus_api/types/internal/objects.hpp"
#include "varjus_api/types/internal/callbacks.hpp"


#include <cassert>

CBufferLinter::CBufferLinter(CProgramInformation* const program, LinterIterator& start, LinterIterator& end, const std::string& filePath)
	: CLinter(start, end), m_oInitialPosition(start), m_sFilePath(filePath), m_pProgram(program) {}
CBufferLinter::~CBufferLinter() = default;

static Success AddInstruction(RuntimeBlock&& block, const WeakScope& scope)
{
	if (block) { //nullptr when no initializer

		if (auto s = scope.lock()) {
			s->AddInstruction(std::move(block));
		} else {
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
		return AddInstruction(linter.ToRuntimeObject(), ctx.scope);
	else
		return success;
}

Success CBufferLinter::LintOperator(const CLinterContext& ctx)
{
	// a new scope
	if ((*ctx.m_iterPos)->IsOperator(p_curlybracket_open)) {
		return LintScope(ctx);
	}

	// otherwise a normal expression
	return Lint<CLinterExpression>(ctx);
}
Success CBufferLinter::LintScope(const CLinterContext& ctx)
{

	if (ctx.memory->IsGlobalMemory()) {
		ctx.m_pModule->PushError("unnamed scopes are not allowed in the global scope", (*ctx.m_iterPos)->m_oSourcePosition);
		return failure;
	}
	auto s = ctx.scope.lock();
	if (!s) {
		return failure;
	}

	std::shared_ptr<CScope> thisScope = s->CreateScope();

	CScopeLinter scopeLinter(ctx.m_iterPos, ctx.m_iterEnd, thisScope, ctx.memory);
	if (!scopeLinter.Parse())
		return failure;

	s->AddInstructions(thisScope->MoveInstructions());
	return success;

}
Success CBufferLinter::LintFunctionAmbiguity(const CLinterContext& ctx)
{
	// a new function
	if(!ctx.memory->IsStack())
		return Lint<CFunctionLinter>(ctx);

	// otherwise a lambda
	return Lint<CLinterExpression>(ctx);
}
Success CBufferLinter::LintToken(const CLinterContext& ctx)
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
	case tt_uint:
	case tt_double:
	case tt_string:
	case tt_fmt_string:
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
	case tt_repeat:
		return Lint<CRepeatStatementLinter>(ctx);
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
		ctx.m_pModule->PushError("unexpected token: " + (*ctx.m_iterPos)->Source(), (*ctx.m_iterPos)->m_oSourcePosition);
		return failure;
	}

}


Success CBufferLinter::Parse()
{
	if (!HoistFile())
		return failure;

	m_iterPos = m_oInitialPosition;

	return LintFile();
}

static void DeclareGlobalObjects(CMemory* m_pOwner, CScope* const scope)
{
	auto info = m_pOwner->GetProgramInformation();

	if(!info || !info->m_oBuiltInObjects)
		return m_pOwner->GetModule()->PushError("internal bug in DeclareGlobalObjects: !info || !info->m_oBuiltInObjects");

	for (auto& [k, v] : info->m_oBuiltInObjects->Iterator()) {
		m_pOwner->m_VariableManager->DeclareVariable(k);
		if (!scope->DeclareVariable(k))
			return m_pOwner->GetModule()->PushError("internal bug in DeclareGlobalObjects");
	}
}
static void DeclareBuiltInFunctions(CMemory* m_pOwner)
{
	auto info = m_pOwner->GetProgramInformation();

	if (!info || !info->m_oBuiltInFunctions)
		return m_pOwner->GetModule()->PushError("internal bug in DeclareGlobalObjects: !info || !info->m_oBuiltInObjects");

	for (auto& [k, v] : info->m_oBuiltInFunctions->Iterator()) {
		auto& [callable, numArgs] = v;
		m_pOwner->m_FunctionManager->DeclareFunction(k, m_pOwner->m_FunctionManager->GetFunctionCount());
		m_pOwner->GetModule()->AddFunction(std::make_unique<CBuiltInRuntimeFunction>(k, callable, numArgs));
	}
}
Success CBufferLinter::HoistFile()
{
	m_pHoister = std::make_unique<CHoister>();

	// no need to give a module index to hoisted files
	auto mod = std::make_unique<CModule>(m_sFilePath);

	CMemory globalMemory(m_pProgram, nullptr, mod.get());
	auto globalScope = std::make_shared<CScope>(&globalMemory);

	DeclareBuiltInFunctions(&globalMemory);
	DeclareGlobalObjects(&globalMemory, globalScope.get());

	CLinterContext ctx{
		.m_iterPos = m_iterPos,
		.m_iterEnd = m_iterEnd,
		.scope = globalScope,
		.memory = &globalMemory,
		.m_pModule = mod.get(),
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

Success CBufferLinter::LintFile()
{
	m_pModule = m_pProgram->GetModules()->CreateNewModule(m_sFilePath);

	CMemory globalMemory(m_pProgram, nullptr, m_pModule);
	globalMemory.m_pHoister = m_pHoister.get();

	auto globalScope = std::make_shared<CScope>(&globalMemory);

	DeclareBuiltInFunctions(&globalMemory);
	DeclareGlobalObjects(&globalMemory, globalScope.get());

	CLinterContext ctx{
		.m_iterPos = m_iterPos,
		.m_iterEnd = m_iterEnd,
		.scope = globalScope,
		.memory = &globalMemory,
		.m_pModule = m_pModule,
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

