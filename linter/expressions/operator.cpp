#include "operator.hpp"
#include "expression.hpp"


#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/error.hpp"
#include "linter/expressions/ast.hpp"
#include "globalEnums.hpp"

#include <cassert>

Punctuation CLinterOperator::GetPunctuation() const noexcept { return m_pToken->m_ePunctuation; }

std::string CLinterOperator::ToString() const noexcept
{
	assert(m_pToken != nullptr && m_ePriority != op_failure);
	return m_pToken->Source();
}

CLinterOperatorParser::CLinterOperatorParser(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, 
	CMemory* const stack) : CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CLinterOperatorParser::~CLinterOperatorParser() = default;

Success CLinterOperatorParser::ParseOperator(std::optional<PairMatcher>& eoe, 
	CExpressionList* expression, EvaluationType evalType)
{

	if (EndOfExpression(eoe))
		return failure;

	if (IsEndOfBuffer() || !CheckOperator()) {
		CLinterErrors::PushError("unexpected end of expression: " + (*m_iterPos)->Source(), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	auto& iterPos = *m_iterPos;
	const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(*iterPos);


	m_pToken = &asPunctuation;

	if ((*m_iterPos)->IsOperator(p_comma)) {

		if (evalType == evaluate_singular)
			return failure;

		if (!ParseSequence(eoe, expression))
			return failure;

	}

	if (!IsOperator(asPunctuation)) {
		CLinterErrors::PushError("unexpected end of expression: " + iterPos->Source(), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);
	return success;
}

Success CLinterOperatorParser::ParseSequence(std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression)
{
	// don't evaluate a sequence when parsing a list
	const auto parsingList = m_oEndOfExpression && m_oEndOfExpression->IsClosing(p_comma);

	if (parsingList)
		return failure;


	std::advance(m_iterPos, 1);
	auto nextExpr = CLinterExpression(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	expression->m_pNext = std::make_unique<CExpressionList>();
	if (!nextExpr.ParseInternal(m_oEndOfExpression, expression->m_pNext.get()))
		return failure;

	
	//time to dip
	if (m_oEndOfExpression && EndOfExpression(m_oEndOfExpression)) {
		std::advance(m_iterPos, 1);
		return failure;
	}

	return failure;
}
bool CLinterOperatorParser::CheckOperator() const
{

	const auto& iterPos = *m_iterPos;
	return iterPos->IsOperator();
}

bool CLinterOperatorParser::IsOperator(const CPunctuationToken& token) const noexcept
{
	return token.m_ePriority >= op_assignment && token.m_ePriority <= op_multiplicative;
}

OperatorPriority CLinterOperatorParser::GetPriority() const noexcept
{
	assert(m_pToken != nullptr);
	return m_pToken->m_ePriority;
}
bool CLinterOperatorParser::EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept
{
	assert(m_iterPos != m_iterEnd);

	if (!eoe)
		return (*m_iterPos)->IsOperator(p_semicolon);

	if (!(*m_iterPos)->IsOperator())
		return false;

	return eoe->IsClosing(dynamic_cast<const CPunctuationToken*>(*m_iterPos)->m_ePunctuation);
}