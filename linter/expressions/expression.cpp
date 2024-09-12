#include "expression.hpp"
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

CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end) {

	assert(m_iterPos != m_iterEnd);

}
CLinterExpression::~CLinterExpression() = default;

Success CLinterExpression::ParseExpression()
{
	Success status = failure;

	do {

		//the previous token was an operator, so we need an operand
		if (EndOfExpression() && !m_oSubExpressions.empty()) {
			CLinterErrors::PushError("Expected an operand, but found " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		auto subExpression = std::make_unique<CLinterSubExpression>(m_iterPos, m_iterEnd);
		status = subExpression->ParseSubExpression();
		m_oSubExpressions.emplace_back(std::move(subExpression));

	} while (status == success);

	assert(m_oSubExpressions.size() > 0u);
	return success;
}
bool CLinterExpression::EndOfExpression() const noexcept
{
	assert(m_iterPos != m_iterEnd);
	return (*m_iterPos)->IsOperator(p_semicolon);
}