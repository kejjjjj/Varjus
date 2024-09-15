#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"

#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "declarations/stack.hpp"

#include <iostream>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end) : CLinter(start, end)
{
}

void LintExpression(LinterIterator& start, LinterIterator& end, CMemoryData* const stack)
{
	CLinterExpression linter(start, end, stack);
	if (linter.ParseExpression())
		std::cout << linter.SortedToString() << '\n';

}

void LintDeclaration(LinterIterator& start, LinterIterator& end, CMemoryData* const stack)
{
	CVariableDeclaration linter(start, end, stack);
	if (linter.ParseDeclaration())
		std::cout << "variable declared\n";

}

Success CFileLinter::ParseFile()
{

	CStack stack;

	while (!IsEndOfBuffer()) {

		switch ((*m_iterPos)->Type()) {
		case t_declaration:
			LintDeclaration(m_iterPos, m_iterEnd, &stack);
			[[fallthrough]]; //fall through because we want to get the initializer
		case t_int:
		case t_double:
		case t_string:
		case t_name:
		case t_operator:
			LintExpression(m_iterPos, m_iterEnd, &stack);
			break;

		case t_error:
		default:
			CLinterErrors::PushError("Unexpected token", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		std::advance(m_iterPos, 1);
	}

	return success;

}
