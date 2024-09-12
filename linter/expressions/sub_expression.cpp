#include "sub_expression.hpp"
#include "operand.hpp"
#include "operator.hpp"

#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "globalEnums.hpp"

#include <cassert>

CLinterSubExpression::CLinterSubExpression(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end) {

	assert(m_iterPos != m_iterEnd);

}
CLinterSubExpression::~CLinterSubExpression() = default;

Success CLinterSubExpression::ParseSubExpression()
{
	//empty subexpression
	if (EndOfExpression())
		return failure;

	CLinterOperand lhsExpression(m_iterPos, m_iterEnd);
	if (!lhsExpression.ParseOperand())
		return failure;

	m_oLhsOperand = std::make_unique<CLinterOperand>(lhsExpression);

	//only one operand
	if (EndOfExpression())
		return failure;

	CLinterOperatorParser cOperator(m_iterPos, m_iterEnd);
	if (!cOperator.ParseOperator())
		return failure;

	m_oOperator = std::make_unique<CLinterOperatorParser>(cOperator);

	//CLinterOperand rhsExpression(m_iterPos, m_iterEnd);
	//if (!rhsExpression.ParseOperand())
	//	return failure;

	//m_oRhsOperand = std::make_unique<CLinterOperand>(rhsExpression);

	return success;
}

bool CLinterSubExpression::EndOfExpression() const noexcept
{
	assert(m_iterPos != m_iterEnd);
	return (*m_iterPos)->IsOperator(p_semicolon);
}
