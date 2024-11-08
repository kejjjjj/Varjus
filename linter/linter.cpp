#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"

#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "declarations/stack.hpp"
#include "functions/function.hpp"
#include "scopes/scope.hpp"
#include "statements/while/while.hpp"
#include "statements/if/if.hpp"
#include "statements/if/else.hpp"
#include "statements/return/return.hpp"

#include <iostream>
#include <cassert>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end) : CLinter(start, end)
{
}

static Success AddInstruction(LinterIterator& pos, RuntimeBlock&& block, const WeakScope& scope)
{
	if (block) { //nullptr when no initializer

		if (auto s = scope.lock()) {
			s->AddInstruction(std::move(block));
		}
		else {
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

	return AddInstruction(start, linter.ToRuntimeObject(), scope);
}

Success CFileLinter::LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	// a new scope
	if ((*start)->IsOperator(p_curlybracket_open)) {
		CScopeLinter scopeLinter(start, end, scope, memory);
		return scopeLinter.Parse();
	}

	// otherwise a normal expression
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
		return LintAddInstruction<CFunctionLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_if:
		return LintAddInstruction<CIfStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_else:
		return Lint<CElseStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_while:
		return LintAddInstruction<CWhileStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_return:
		return LintAddInstruction<CReturnStatementLinter>(m_iterPos, m_iterEnd, scope, memory);
	case tt_error:
	default:
		CLinterErrors::PushError("unexpected token: " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

}
Success CFileLinter::ParseFile()
{

	m_pFile = std::make_unique<CFileRuntimeData>();
	CMemory globalMemory(m_pFile.get());
	auto globalScope = std::make_shared<CScope>(&globalMemory);

	while (!IsEndOfBuffer()) {
		if (!LintToken(m_iterPos, m_iterEnd, globalScope, &globalMemory))
			break;

		if (IsEndOfBuffer())
			break;

		std::advance(m_iterPos, 1);
	}

	return success;
}

CFileRuntimeData* CFileLinter::GetRuntimeInformation() const noexcept
{
	return m_pFile.get();
}


