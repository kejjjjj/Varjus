#include "expression.hpp"
#include "sub_expression.hpp"
#include "operand.hpp"
#include "operator.hpp"

#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/declarations/stack.hpp"

#include "ast.hpp"

#include "globalEnums.hpp"

#include <cassert>
#include <algorithm>

CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack) 
	: m_iterPos(pos), m_iterEnd(end), m_pOwner(stack) {

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

		auto subExpression = std::make_unique<CLinterSubExpression>(m_iterPos, m_iterEnd, m_pOwner);
		status = subExpression->ParseSubExpression();
		m_oSubExpressions.emplace_back(std::move(subExpression));

	} while (status == success);
	assert(m_oSubExpressions.size() > 0u);

	Sort();

	return success;
}
bool CLinterExpression::EndOfExpression() const noexcept
{
	assert(m_iterPos != m_iterEnd);
	return (*m_iterPos)->IsOperator(p_semicolon);
}

void CLinterExpression::Sort()
{
	std::vector<CLinterOperand*> operands;
	std::vector<CLinterOperator*> operators;

	for (auto& subExpression : m_oSubExpressions) {
		operands.emplace_back(&*subExpression->m_oLhsOperand);
		if(subExpression->m_oOperator)
			operators.emplace_back(&*subExpression->m_oOperator);
	}	

	assert(operands.size() == operators.size() + 1u);
	m_pAST = AbstractSyntaxTree::CreateAST(operands, operators);

}
std::string CLinterExpression::ToString() const noexcept
{
	assert(!m_oSubExpressions.empty());

	std::string result;
	for (const auto& subExpression : m_oSubExpressions) {
		result += subExpression->m_oLhsOperand->ToString() + ' ';
		if (subExpression->m_oOperator != nullptr) {
			result += subExpression->m_oOperator->ToString() + ' ';
		}
	}
	return result;
}

std::string CLinterExpression::SortedToString() const noexcept
{
	assert(m_pAST != nullptr);
	return m_pAST->ToString();
}