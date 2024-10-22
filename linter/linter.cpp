#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"

#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "declarations/stack.hpp"
#include "functions/function.hpp"

#include <iostream>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end) : CLinter(start, end)
{
}

void CFileLinter::LintExpression(LinterIterator& start, LinterIterator& end, CMemory* const stack)
{
	CLinterExpression linter(start, end, stack);
	if (linter.ParseExpression()) {
		//std::cout << linter.SortedToString() << '\n';
		linter.QuickEvalAST();
	}

}

void CFileLinter::LintDeclaration(LinterIterator& start, LinterIterator& end, CMemory* const stack)
{
	CVariableDeclaration linter(start, end, stack);
	if (linter.ParseDeclaration())
		std::cout << "variable declared\n";

}

void CFileLinter::LintFunction(LinterIterator& start, LinterIterator& end, CMemory* const stack)
{
	CFunctionLinter linter(start, end, stack);
	if (linter.ParseFunctionDeclaration())
		std::cout << "variable declared\n";

}
Success  CFileLinter::LintToken(LinterIterator& m_iterPos, LinterIterator& m_iterEnd, CMemory* const stack)
{
	if (m_iterPos == m_iterEnd)
		return failure;

	switch ((*m_iterPos)->Type()) {
	case t_declaration:
		LintDeclaration(m_iterPos, m_iterEnd, stack);
		[[fallthrough]]; //fallthrough because we want to get the initializer
	case t_int:
	case t_double:
	case t_string:
	case t_name:
	case t_operator:
		LintExpression(m_iterPos, m_iterEnd, stack);
		break;
	case t_fn:
		LintFunction(m_iterPos, m_iterEnd, stack);
		break;
	case t_error:
	default:
		CLinterErrors::PushError("Unexpected token", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	return success;
}
Success CFileLinter::ParseFile()
{

	CStack stack;

	while (!IsEndOfBuffer()) {

		if (!LintToken(m_iterPos, m_iterEnd, &stack))
			break;

		std::advance(m_iterPos, 1);
	}

	return success;
}


/***********************************************************************
 > 
***********************************************************************/
std::weak_ptr<CScope> CFileLinter::GetCurrentScope() {
	return m_pCurrentScope;
}
std::weak_ptr<CScope> CFileLinter::m_pCurrentScope{};




