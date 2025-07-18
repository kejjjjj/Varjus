#include "sub_expression.hpp"
#include "expression.hpp"
#include "operand/operand.hpp"
#include "operator.hpp"

#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/functions/stack.hpp"
#include "linter/modules/module.hpp"
#include "varjus_api/internal/globalEnums.hpp"

#include <cassert>

using namespace Varjus;

CLinterSubExpression::CLinterSubExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope,
	CMemory* const stack, std::optional<PairMatcher>& eoe)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack), m_oEndOfExpression(eoe) {

	assert(m_iterPos != m_iterEnd);
}
CLinterSubExpression::~CLinterSubExpression() = default;

Varjus::Success CLinterSubExpression::ParseSubExpression(std::optional<PairMatcher>& eoe,
	CExpressionList* expression, EvaluationType evalType)
{
	//empty subexpression
	if (EndOfExpression())
		return failure;

	m_oLhsOperand = std::make_unique<CLinterOperand>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!m_oLhsOperand->ParseOperand(eoe)) {
		m_oLhsOperand.reset();
		return failure;
	}

	//only one operand
	if (EndOfExpression())
		return failure;

	if (m_iterPos == m_iterEnd) {
		m_pOwner->GetModule()->PushError(VSL("unexpected end of buffer"));
		return failure;
	}

	CLinterOperatorParser cOperator(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!cOperator.ParseOperator(eoe, expression, evalType))
		return failure;

	m_oOperator = std::make_unique<CLinterOperator>(cOperator.GetPriority(), cOperator.GetToken());
	return success;
}

bool CLinterSubExpression::EndOfExpression() const noexcept
{
	if (m_iterPos == m_iterEnd)
		return false; //let it fail!

	assert(m_iterPos != m_iterEnd);
	if (!m_oEndOfExpression) {
		return (*m_iterPos)->IsOperator(p_semicolon);
	}

	if (!(*m_iterPos)->IsOperator())
		return false;

	return m_oEndOfExpression->IsClosing(dynamic_cast<const CPunctuationToken*>(*m_iterPos)->m_ePunctuation);
}
OperatorPriority CLinterSubExpression::GetPriority() const noexcept
{
	assert(m_oOperator != nullptr);
	return m_oOperator->GetPriority();
}
