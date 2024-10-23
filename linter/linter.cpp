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

void CFileLinter::LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
{
	if ((*start)->IsOperator(p_curlybracket_open)) {
		CScopeLinter scopeLinter(start, end, scope, stack);
		scopeLinter.ParseScope();
		return;
	}

	return LintExpression(start, end, scope, stack);
}
void CFileLinter::LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
{
	CLinterExpression linter(start, end, scope, stack);
	if (linter.ParseExpression()) {
		//linter.QuickEvalAST();
	}

}

void CFileLinter::LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
{
	CVariableDeclaration linter(start, end, scope, stack);
	if (linter.ParseDeclaration())
		std::cout << "variable declared\n";

}

void CFileLinter::LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
{
	CFunctionLinter linter(start, end, scope, stack);
	if (linter.ParseFunctionDeclaration())
		std::cout << "function declared\n";

}
Success CFileLinter::LintToken(LinterIterator& m_iterPos, LinterIterator& m_iterEnd, const WeakScope& scope, CMemory* const stack)
{
	if (m_iterPos == m_iterEnd)
		return failure;

	switch ((*m_iterPos)->Type()) {
	case t_declaration:
		LintDeclaration(m_iterPos, m_iterEnd, scope, stack);
		[[fallthrough]];
	case t_int:
	case t_double:
	case t_string:
	case t_name:
		LintExpression(m_iterPos, m_iterEnd, scope, stack);
		break;
	case t_operator:
		LintOperator(m_iterPos, m_iterEnd, scope, stack);
		break;
	case t_fn:
		LintFunction(m_iterPos, m_iterEnd, scope, stack);
		break;
	case t_error:
	default:
		CLinterErrors::PushError("Unexpected token", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	return m_iterPos != m_iterEnd ? success : failure;
}
Success CFileLinter::ParseFile()
{

	CStack stack;

	auto scope = std::make_shared<CScope>(&stack);

	while (!IsEndOfBuffer()) {

		if (!LintToken(m_iterPos, m_iterEnd, scope, &stack))
			break;

		std::advance(m_iterPos, 1);
	}

	return success;
}



