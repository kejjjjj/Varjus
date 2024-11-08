#include "operator.hpp"

#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/error.hpp"
#include "globalEnums.hpp"

#include <cassert>

Punctuation CLinterOperator::GetPunctuation() const noexcept { return m_pToken->m_ePunctuation; }

std::string CLinterOperator::ToString() const noexcept
{
	assert(m_pToken != nullptr && m_ePriority != op_failure);
	return m_pToken->Source();
}

CLinterOperatorParser::CLinterOperatorParser(LinterIterator& pos, LinterIterator& end) : CLinterSingle(pos, end)
{
	assert(m_iterPos != m_iterEnd);
}
CLinterOperatorParser::~CLinterOperatorParser() = default;

Success CLinterOperatorParser::ParseOperator()
{

	if (IsEndOfBuffer() || !CheckOperator()) {
		CLinterErrors::PushError("unexpected end of expression: " + (*m_iterPos)->Source(), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	auto& iterPos = *m_iterPos;
	const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(*iterPos);

	if ((*m_iterPos)->IsOperator(p_comma))
		return failure;

	if (!IsOperator(asPunctuation)) {
		CLinterErrors::PushError("unexpected end of expression: " + iterPos->Source(), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_pToken = &asPunctuation;
	std::advance(m_iterPos, 1);

	return success;
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

std::string CLinterOperatorParser::ToString() const noexcept
{
	return m_pToken ? m_pToken->Source() : "";

}