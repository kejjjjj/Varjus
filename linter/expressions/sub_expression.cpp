#include "sub_expression.hpp"
#include "expression.hpp"
#include "operand.hpp"
#include "operator.hpp"

#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/declarations/stack.hpp"
#include "globalEnums.hpp"

#include <cassert>

CLinterSubExpression::CLinterSubExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope,
	CMemory* const stack, std::optional<PairMatcher>& eoe)
	: m_iterPos(pos), m_iterEnd(end), m_pScope(scope), m_pOwner(stack), m_oEndOfExpression(eoe) {

	assert(m_iterPos != m_iterEnd);
}
CLinterSubExpression::~CLinterSubExpression() = default;

Success CLinterSubExpression::ParseSubExpression()
{
	//empty subexpression
	if (EndOfExpression())
		return failure;

	m_oLhsOperand = std::make_unique<CLinterOperand>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!m_oLhsOperand->ParseOperand()) {
		m_oLhsOperand.reset();
		return failure;
	}

	//only one operand
	if (EndOfExpression())
		return failure;

	CLinterOperatorParser cOperator(m_iterPos, m_iterEnd);
	if (!cOperator.ParseOperator())
		return failure;

	m_oOperator = std::make_unique<CLinterOperator>(cOperator.GetPriority(), cOperator.GetToken());
	return success;
}

bool CLinterSubExpression::EndOfExpression() const noexcept
{
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
