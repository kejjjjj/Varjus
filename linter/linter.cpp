#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"

#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "declarations/stack.hpp"
#include "functions/function.hpp"
#include "scopes/scope.hpp"

#include <iostream>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end) : CLinter(start, end)
{
}

Success CFileLinter::LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	// a new scope
	if ((*start)->IsOperator(p_curlybracket_open)) {
		CScopeLinter scopeLinter(start, end, scope, memory);
		scopeLinter.ParseScope();
		return success;
	}

	// otherwise a normal expression
	return LintExpression(start, end, scope, memory);
}
Success CFileLinter::LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	CLinterExpression linter(start, end, scope, memory);
	if (!linter.ParseExpression()) 
		return failure;

	[[maybe_unused]] const auto rtObject = linter.ToRuntimeObject();
	return success;
}

Success CFileLinter::LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	CVariableDeclaration linter(start, end, scope, memory);
	if (!linter.ParseDeclaration())
		return failure;

	[[maybe_unused]] const auto rtObject = linter.ToRuntimeObject();
	return success;
}

Success CFileLinter::LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	CFunctionLinter linter(start, end, scope, memory);
	if (!linter.ParseFunction())
		return failure;

	return success;

}
Success CFileLinter::LintToken(LinterIterator& m_iterPos, LinterIterator& m_iterEnd, const WeakScope& scope, CMemory* const memory)
{
	if (m_iterPos == m_iterEnd)
		return failure;

	switch ((*m_iterPos)->Type()) {
	case t_declaration:
		return LintDeclaration(m_iterPos, m_iterEnd, scope, memory);
	case t_int:
	case t_double:
	case t_string:
	case t_name:
		return LintExpression(m_iterPos, m_iterEnd, scope, memory);
	case t_operator:
		return LintOperator(m_iterPos, m_iterEnd, scope, memory);
	case t_fn:
		return LintFunction(m_iterPos, m_iterEnd, scope, memory);
	case t_error:
	default:
		CLinterErrors::PushError("Unexpected token", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

}
Success CFileLinter::ParseFile()
{

	CMemory globalMemory;

	auto scope = std::make_shared<CScope>(&globalMemory);

	while (!IsEndOfBuffer()) {

		if (!LintToken(m_iterPos, m_iterEnd, scope, &globalMemory))
			break;

		std::advance(m_iterPos, 1);
	}

	return success;
}



