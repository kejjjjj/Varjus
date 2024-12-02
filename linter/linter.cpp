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
#include "statements/for/for.hpp"
#include "statements/while/while.hpp"
#include "statements/if/if.hpp"
#include "statements/if/else.hpp"
#include "statements/return/return.hpp"
#include "statements/control/control.hpp"

#include <cassert>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end, CProgramContext* const context) 
	: CLinter(start, end), m_pContext(context), m_oInitialPosition(start)
{
	assert(m_pContext);
}
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

template<typename Linter> Success Lint(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{

	Linter linter(start, end, scope, memory);
	if (!linter.Parse())
		return failure;

	return success;
}

template<typename Linter> 
Success LintAddInstruction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{

	Linter linter(start, end, scope, memory);
	if (!linter.Parse())
		return failure;

	if constexpr (std::is_same_v<CFunctionLinter, Linter>)
		return success;
	else
		return AddInstruction(start, linter.ToRuntimeObject(), scope);
}

Success CFileLinter::LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	// a new scope
	if ((*start)->IsOperator(p_curlybracket_open)) {
		return LintScope(start, end, scope, memory);
	}

	// otherwise a normal expression
	return LintAddInstruction<CLinterExpression>(start, end, scope, memory);
}
Success CFileLinter::LintScope(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{

	if (memory->IsGlobalMemory()) {
		CLinterErrors::PushError("unnamed scopes are not allowed in the global scope", (*start)->m_oSourcePosition);
		return failure;
	}
	auto s = scope.lock();
	if (!s) {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", (*start)->m_oSourcePosition);
		return failure;
	}

	std::shared_ptr<CScope> thisScope = s->CreateScope();

	CScopeLinter scopeLinter(start, end, thisScope, memory);
	if (!scopeLinter.Parse())
		return failure;

	s->AddInstructions(thisScope->MoveInstructions());
	return success;

}
Success CFileLinter::LintFunctionAmbiguity(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	// a new function
	if(!memory->IsStack())
		return LintAddInstruction<CFunctionLinter>(start, end, scope, memory);

	// otherwise a lambda
	return LintAddInstruction<CLinterExpression>(start, end, scope, memory);
}
Success CFileLinter::LintToken(LinterIterator& m_iterPos, LinterIterator& m_iterEnd, const WeakScope& scope, CMemory* const memory)
{
	if (m_iterPos == m_iterEnd)
		return failure;

	switch ((*m_iterPos)->Type()) {
	case tt_declaration:
		return LintAddInstruction<CVariableDeclarationLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_undefined:
	case tt_false:
	case tt_true:
	case tt_int:
	case tt_double:
	case tt_string:
	case tt_name:
		return LintAddInstruction<CLinterExpression>(m_iterPos, m_iterEnd, scope, memory);
	case tt_operator:
		return LintOperator(m_iterPos, m_iterEnd, scope, memory);
	case tt_fn:
		return LintFunctionAmbiguity(m_iterPos, m_iterEnd, scope, memory);
	case tt_if:
		return LintAddInstruction<CIfStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_else:
		return Lint<CElseStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_for:
		return LintAddInstruction<CForStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_while:
		return LintAddInstruction<CWhileStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_return:
		return LintAddInstruction<CReturnStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_break:
	case tt_continue:
		return LintAddInstruction<CLoopControlStatement>(m_iterPos, m_iterEnd, scope, memory);
	case tt_error:
	default:
		CLinterErrors::PushError("unexpected token: " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

}

Success CFileLinter::ParseFile()
{
	if (!HoistFile())
		return failure;

	m_iterPos = m_oInitialPosition;
	m_pContext->Reset();
	m_pFile.reset();


	return LintFile();
}

Success CFileLinter::HoistFile()
{
	m_pHoister = std::make_unique<CHoister>();
	m_pFile = std::make_unique<CFileRuntimeData>();

	CMemory globalMemory(nullptr, m_pFile.get(), m_pContext);
	auto globalScope = std::make_shared<CScope>(&globalMemory);

	while (!IsEndOfBuffer()) {
		if (!LintToken(m_iterPos, m_iterEnd, globalScope, &globalMemory))
			break;

		if (IsEndOfBuffer())
			break;

		std::advance(m_iterPos, 1);
	}

	for (const auto& func : m_pFile->m_oFunctions) {
		m_pHoister->DeclareFunction(func->GetName());
	}

	return success;
}
Success CFileLinter::LintFile()
{
	m_pFile = std::make_unique<CFileRuntimeData>();
	CMemory globalMemory(nullptr, m_pFile.get(), m_pContext);
	globalMemory.m_pHoister = m_pHoister.get();

	auto globalScope = std::make_shared<CScope>(&globalMemory);

	while (!IsEndOfBuffer()) {
		if (!LintToken(m_iterPos, m_iterEnd, globalScope, &globalMemory))
			break;

		if (IsEndOfBuffer())
			break;

		std::advance(m_iterPos, 1);
	}

	m_pFile->AddGlobalInstructions(globalScope->MoveInstructions());
	m_pFile->SetGlobalVariableCount(globalMemory.m_VariableManager->GetVariableCount());

	return success;
}

CFileRuntimeData* CFileLinter::GetRuntimeInformation() const noexcept
{
	return m_pFile.get();
}


