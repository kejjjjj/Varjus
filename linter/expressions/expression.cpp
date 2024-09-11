#include "expression.hpp"
#include "operand.hpp"
#include "operator.hpp"

#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "globalEnums.hpp"

#include <cassert>

CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end) {

	assert(m_iterPos != m_iterEnd);

}
CLinterExpression::~CLinterExpression() = default;

Success CLinterExpression::ParseExpression()
{
	//TODO: check for end-of-expression

	CLinterOperand lhsExpression(m_iterPos, m_iterEnd);
	if (!lhsExpression.ParseOperand())
		return failure;

	m_oOperands.emplace_back(std::make_unique<CLinterOperand>(lhsExpression));

	//TODO: check for end-of-expression

	CLinterOperatorParser cOperator(m_iterPos, m_iterEnd);
	if (!cOperator.ParseOperator())
		return failure;

	m_oOperators.emplace_back(std::make_unique<CLinterOperatorParser>(cOperator));

	CLinterOperand rhsExpression(m_iterPos, m_iterEnd);
	if (!rhsExpression.ParseOperand())
		return failure;

	m_oOperands.emplace_back(std::make_unique<CLinterOperand>(rhsExpression));

	m_oOperatorLinks.push_back( CLinterOperator{ 
		.m_pOperator		= &*m_oOperators.back(), 
		.m_oLhsOperand	= &*m_oOperands.at(m_oOperands.size() - 2u),
		.m_oRhsOperand	= &*m_oOperands.at(m_oOperands.size() - 1u),
		});

	return success;
}