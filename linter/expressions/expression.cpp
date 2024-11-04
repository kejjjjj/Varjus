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

CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterExpression::~CLinterExpression() = default;
CExpressionList::CExpressionList() = default;
CExpressionList::~CExpressionList() = default;

std::unique_ptr<AbstractSyntaxTree> CExpressionList::ToMergedAST()
{
	if (!m_pNext)
		return std::move(m_pAST);

	auto newRoot = std::make_unique<OperatorASTNode>(p_comma);
	newRoot->left = std::move(m_pAST);
	newRoot->right = m_pNext->ToMergedAST();
	return newRoot;
}
ExpressionList CExpressionList::ToExpressionList()
{
	ExpressionList list;
	auto pos = this;
	while (pos) {
		assert(pos->m_pAST);
		list.emplace_back(std::make_unique<CRuntimeExpression>(std::move(pos->m_pAST)));
		pos = pos->m_pNext.get();
	}

	return list;
}
Success CLinterExpression::ParseExpression(std::optional<PairMatcher> m_oEndOfExpression, CExpressionList* expression)
{
	Success status = failure;

	if (!expression) {
		m_pEvaluatedExpressions = std::make_unique<CExpressionList>();
	}

	auto actualExpression = expression ? expression : m_pEvaluatedExpressions.get();
	assert(actualExpression);

	do {

		//the previous token was an operator, so we need an operand
		if (EndOfExpression(m_oEndOfExpression) && !m_oSubExpressions.empty()) {
			CLinterErrors::PushError("expected an operand, but found " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		auto subExpression = std::make_unique<CLinterSubExpression>(m_iterPos, m_iterEnd, m_pScope, m_pOwner, m_oEndOfExpression);
		status = subExpression->ParseSubExpression();
		m_oSubExpressions.emplace_back(std::move(subExpression));

		if (!ParseSequence(m_oEndOfExpression, actualExpression))
			return failure;

	} while (status == success);

	if (m_oSubExpressions.empty()) {
		CLinterErrors::PushError("expected an expression", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}


	assert(m_oSubExpressions.size() > 0u);

	if (m_oEndOfExpression && EndOfExpression(m_oEndOfExpression)) {
		std::advance(m_iterPos, 1);
	}

	actualExpression->m_pAST = ToAST();
	return success;
}
Success CLinterExpression::ParseSequence(std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression)
{
	// don't evaluate a sequence when parsing a list
	const auto parsingList = m_oEndOfExpression && m_oEndOfExpression->IsClosing(p_comma);

	if (parsingList)
		return success;

	if ((*m_iterPos)->IsOperator(p_comma)) {

		std::advance(m_iterPos, 1);
		auto nextExpr = CLinterExpression(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

		expression->m_pNext = std::make_unique<CExpressionList>();
		if (!nextExpr.ParseExpression(m_oEndOfExpression, expression->m_pNext.get()))
			return failure;

		//m_pNextExpression = nextExpr.ToAST();
	}

	return success;
}
bool CLinterExpression::EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept
{
	assert(m_iterPos != m_iterEnd);

	if (!eoe) 
		return (*m_iterPos)->IsOperator(p_semicolon);

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
std::unique_ptr<AbstractSyntaxTree> CLinterExpression::ToMergedAST() const
{
	assert(m_pEvaluatedExpressions);
	return m_pEvaluatedExpressions->ToMergedAST();
}
ExpressionList CLinterExpression::ToExpressionList() const
{
	assert(m_pEvaluatedExpressions);
	return m_pEvaluatedExpressions->ToExpressionList();
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
RuntimeBlock CLinterExpression::ToRuntimeObject() const
{
	return std::make_unique<CRuntimeExpression>(ToAST());
}
