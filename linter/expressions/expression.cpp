#include "expression.hpp"
#include "expression_context.hpp"
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
#include <iostream>

CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end, CMemory* const stack) 
	: m_iterPos(pos), m_iterEnd(end), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterExpression::~CLinterExpression() = default;

Success CLinterExpression::ParseExpression(std::optional<PairMatcher> m_oEndOfExpression)
{
	Success status = failure;

	do {

		//the previous token was an operator, so we need an operand
		if (EndOfExpression(m_oEndOfExpression) && !m_oSubExpressions.empty()) {
			CLinterErrors::PushError("Expected an operand, but found " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		auto subExpression = std::make_unique<CLinterSubExpression>(m_iterPos, m_iterEnd, m_pOwner, m_oEndOfExpression);
		status = subExpression->ParseSubExpression();
		m_oSubExpressions.emplace_back(std::move(subExpression));

	} while (status == success);
	assert(m_oSubExpressions.size() > 0u);

	if (m_oEndOfExpression && EndOfExpression(m_oEndOfExpression)) {
		std::advance(m_iterPos, 1);
	}

	return success;
}
bool CLinterExpression::EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept
{
	if (!eoe) {
		assert(m_iterPos != m_iterEnd);
		return (*m_iterPos)->IsOperator(p_semicolon);
	}

	if (!(*m_iterPos)->IsOperator())
		return false;

	return eoe->IsClosing(dynamic_cast<const CPunctuationToken*>(*m_iterPos)->m_ePunctuation);
}

std::unique_ptr<AbstractSyntaxTree> CLinterExpression::ToAST() const
{
	std::vector<CLinterOperand*> operands;
	std::vector<CLinterOperator*> operators;

	for (const auto& subExpression : m_oSubExpressions) {
		operands.emplace_back(&*subExpression->m_oLhsOperand);
		if (subExpression->m_oOperator)
			operators.emplace_back(&*subExpression->m_oOperator);
	}

	assert(operands.size() == operators.size() + 1u);
	return AbstractSyntaxTree::CreateAST(operands, operators);
}

void CLinterExpression::QuickEvalAST()
{

	const auto ast = ToAST();
	const auto result = QuickEvalASTInternal(&*ast);

	std::cout << "QuickEval(): " << result << '\n';
}

int CLinterExpression::QuickEvalASTInternal(const AbstractSyntaxTree* node)
{

	if (!node) {
		assert(false);
		return 0;
	}

	if (node->IsLeaf()) {

		if (node->IsVariable()) {
			const auto var = node->As<const VariableASTNode*>();
			return std::stoi(var->ToStringPolymorphic());
		}

		const auto var = node->As<const ConstantASTNode*>();
		return std::stoi(var->ToStringPolymorphic());
	}

	const auto lhs = QuickEvalASTInternal(&*node->left);
	const auto rhs = QuickEvalASTInternal(&*node->right);

	assert(node->IsOperator());

	const auto oper = node->As<const OperatorASTNode*>();

#pragma pack(push)
#pragma warning(disable : 4062)
	switch (oper->m_ePunctuation) {
	case p_add:
		std::cout << std::format("{} + {} = {}\n", lhs, rhs, lhs + rhs);
		return lhs + rhs;
	case p_sub:
		std::cout << std::format("{} - {} = {}\n", lhs, rhs, lhs - rhs);
		return lhs - rhs;
	case p_multiplication:
		std::cout << std::format("{} * {} = {}\n", lhs, rhs, lhs * rhs);
		return lhs * rhs;
	case p_division:
		std::cout << std::format("{} / {} = {}\n", lhs, rhs, lhs / rhs);
		return lhs / rhs;
	}

	return lhs;
#pragma pack(pop)

}

//void CLinterExpression::Sort()
//{
//	std::vector<CLinterOperand*> operands;
//	std::vector<CLinterOperator*> operators;
//
//	for (auto& subExpression : m_oSubExpressions) {
//		operands.emplace_back(&*subExpression->m_oLhsOperand);
//		if(subExpression->m_oOperator)
//			operators.emplace_back(&*subExpression->m_oOperator);
//	}	
//
//	assert(operands.size() == operators.size() + 1u);
//	//m_pAST = AbstractSyntaxTree::CreateAST(operands, operators);
//
//}
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
	const auto ast = ToAST();
	assert(ast != nullptr);
	return ast->ToString();
}