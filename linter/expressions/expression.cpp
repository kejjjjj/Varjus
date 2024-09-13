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
#include <algorithm>

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
	//sort the subexpressions by priority

	std::vector<CLinterOperand*> operands;
	std::vector<CLinterOperatorParser*> operators;

	for (auto& subExpression : m_oSubExpressions) {
		operands.emplace_back(&*subExpression->m_oLhsOperand);
		if(subExpression->m_oOperator)
			operators.emplace_back(&*subExpression->m_oOperator);
	}	

	assert(operands.size() == operators.size() + 1u);

	m_oSortedSubExpressions = SortIteratively(operands, operators);
}

OperatorIterator CLinterExpression::FindHighestPriorityOperator(std::vector<CLinterOperatorParser*>& operators)
{
	assert(!operators.empty());

	OperatorPriority op{}, nextOp{};
	auto itr1 = operators.begin();
	auto itr2 = std::next(itr1);

	if (itr2 != operators.end()) {
		do {

			op = (*itr1)->GetPriority();
			nextOp = (*itr2)->GetPriority();

			if (nextOp <= op) {
				break;
			}

			std::advance(itr1, 1);
			std::advance(itr2, 1);

		} while (itr2 != operators.end() && nextOp > op);
	}

	return itr1;
}

[[nodiscard]] std::vector<CSortableSubExpression>
CLinterExpression::SortIteratively(std::vector<CLinterOperand*>& operands, std::vector<CLinterOperatorParser*>& operators)
{
	assert(!operands.empty() && !operators.empty());
	std::vector<CSortableSubExpression> sorted;
	CSortableSubExpression subExpression{};

	std::ptrdiff_t index = 0;

	do {
		const auto itr1 = FindHighestPriorityOperator(operators);

		if(subExpression.m_oLhsOperand){

			subExpression.m_oOperator = &**itr1;

			sorted.emplace_back(subExpression);
			operators.erase(itr1);
			operands.erase(operands.begin() + index);

			subExpression.m_oLhsOperand = 0;
			subExpression.m_oOperator = 0;

		}
		else {

			index = std::distance(operators.begin(), itr1);

			if (index < 0)
				CLinterErrors::PushError("Index out of bounds", {});

			subExpression.m_oLhsOperand = &*operands.at(std::size_t(index));
			subExpression.m_oOperator = &**itr1;

			sorted.emplace_back(subExpression);
			operators.erase(itr1);
			operands.erase(operands.begin() + index);

			if(operands.size() > 1u)
				subExpression.m_oLhsOperand = &*operands.at(std::size_t(index));

		}

	} while (!operators.empty());
	
	assert(operands.size() == 1u);

	////add the last operand
	subExpression.m_oOperator = 0;
	subExpression.m_oLhsOperand = operands.back();
	sorted.emplace_back(subExpression);

	return sorted;

}
std::string CLinterExpression::ToString() const noexcept
{
	assert(!m_oSubExpressions.empty());

	std::string result;
	for (const auto& subExpression : m_oSubExpressions) {
		result += subExpression->m_oLhsOperand->ToString() + ' ';
		if (subExpression->m_oOperator != nullptr) {
			result += subExpression->m_oOperator->ToString() + ' ';
			//result += subExpression->m_oRhsOperand->ToString();
		}
	}
	return result;
}
std::string CLinterExpression::SortedToString() const noexcept
{
	assert(!m_oSortedSubExpressions.empty());

	std::string result;
	for (const auto& subExpression : m_oSortedSubExpressions) {
		result += subExpression.m_oLhsOperand->ToString() + ' ';
		if (subExpression.m_oOperator != nullptr) {
			result += subExpression.m_oOperator->ToString() + ' ';
			//result += subExpression.m_oRhsOperand->ToString() + ' ';

		}
	}
	return result;
}
