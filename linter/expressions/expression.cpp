#include "ast.hpp"
#include "expression.hpp"
#include "expression_context.hpp"
#include "varjus_api/internal/globalEnums.hpp"
#include "identifier.hpp"
#include "linter/error.hpp"
#include "linter/functions/stack.hpp"
#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "operand/operand.hpp"
#include "operator.hpp"
#include "sub_expression.hpp"
#include "unary.hpp"
#include "linter/modules/module.hpp"

#include <algorithm>
#include <cassert>

using namespace Varjus;


CLinterExpression::CLinterExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterExpression::~CLinterExpression() = default;
CExpressionList::CExpressionList() = default;
CExpressionList::~CExpressionList() = default;

ASTNode CExpressionList::ToMergedAST()
{
	if (!m_pNext)
		return std::move(m_pAST);

	auto newRoot = std::make_unique<OperatorASTNode>(m_pAST->GetCodePosition(), p_comma);
	newRoot->left = std::move(m_pAST);
	newRoot->right = m_pNext->ToMergedAST();
	return newRoot;
}
__ExpressionList CExpressionList::ToExpressionList()
{
	__ExpressionList list;
	auto pos = this;
	while (pos) {
		assert(pos->m_pAST);
		list.emplace_back(std::move(pos->m_pAST));
		pos = pos->m_pNext.get();
	}

	return list;
}
Varjus::Success CLinterExpression::Parse(std::optional<PairMatcher> m_oEndOfExpression, 
	CExpressionList* expression, EvaluationType evalType)
{
	return ParseInternal(m_oEndOfExpression, expression, evalType);
}
Varjus::Success CLinterExpression::ParseInternal(std::optional<PairMatcher>& m_oEndOfExpression, 
	CExpressionList* expression, EvaluationType evalType)
{
	Varjus::Success status = failure;

	if(EndOfExpression(m_oEndOfExpression))
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);

	if (!expression) {
		m_pEvaluatedExpressions = std::make_unique<CExpressionList>();
	}

	auto actualExpression = expression ? expression : m_pEvaluatedExpressions.get();
	assert(actualExpression);

	do {

		//the previous token was an operator, so we need an operand
		if (EndOfExpression(m_oEndOfExpression) && !m_oSubExpressions.empty()) {
			m_pOwner->GetModule()->PushError(VSL("expected an operand, but found ") + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		auto subExpression = std::make_unique<CLinterSubExpression>(m_iterPos, m_iterEnd, m_pScope, m_pOwner, m_oEndOfExpression);
		status = subExpression->ParseSubExpression(m_oEndOfExpression, actualExpression, evalType);
		m_oSubExpressions.emplace_back(std::move(subExpression));

	} while (status == success);

	if (m_oSubExpressions.empty()) {
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}


	assert(m_oSubExpressions.size() > 0u);

	//singular does NOT want to iterate over the eoe symbol
	if (evalType == evaluate_everything && m_oEndOfExpression && EndOfExpression(m_oEndOfExpression)) {
		std::advance(m_iterPos, 1);
		m_oEndOfExpression = std::nullopt;
	}

	// no need to create the ASTs when hoisting
	if(!m_pOwner->IsHoisting())
		actualExpression->m_pAST = ToAST();

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

ASTNode CLinterExpression::ToAST() const
{
	if (m_pOwner->IsHoisting())
		return nullptr;

	std::vector<CLinterOperand*> operands;
	std::vector<CLinterOperator*> operators;

	for (const auto& subExpression : m_oSubExpressions) {
		operands.emplace_back(&*subExpression->m_oLhsOperand);
		if (subExpression->m_oOperator)
			operators.emplace_back(&*subExpression->m_oOperator);
	}

	assert(operands.size() == operators.size() + 1u);
	return AbstractSyntaxTree::CreateAST(m_pOwner, operands, operators);
}
ASTNode CLinterExpression::ToMergedAST() const
{
	if (m_pOwner->IsHoisting())
		return nullptr;

	assert(m_pEvaluatedExpressions);
	return m_pEvaluatedExpressions->ToMergedAST();
}
__ExpressionList CLinterExpression::ToExpressionList() const
{
	if (m_pOwner->IsHoisting())
		return {};

	assert(m_pEvaluatedExpressions);
	return m_pEvaluatedExpressions->ToExpressionList();
}
__RuntimeBlock CLinterExpression::ToRuntimeObject() const
{
	if (m_pOwner->IsHoisting())
		return nullptr;

	assert(!m_pOwner->IsHoisting());
	return std::make_unique<CRuntimeExpression>(ToMergedAST());
}

bool CLinterExpression::CanBeDiscarded(const ASTNode& node) noexcept
{
	if (node->IsMeaningful())
		return false;

	if (node->left)
		return CanBeDiscarded(node->left);

	if (node->right)
		return CanBeDiscarded(node->right);

	return true;
}
