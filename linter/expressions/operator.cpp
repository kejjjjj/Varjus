#include "operator.hpp"

#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/error.hpp"
#include "globalEnums.hpp"

#include <cassert>

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

	if (m_iterPos == m_iterEnd){
		CLinterErrors::PushError("Expected operator, but reached end of file", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	if (!CheckOperator()) {
		return failure;
	}

	auto& iterPos = *m_iterPos;
	const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(*iterPos);

	if (!IsOperator(asPunctuation)) {
		CLinterErrors::PushError("Expected operator, but found " + iterPos->Source(), iterPos->m_oSourcePosition);
		return failure;
	}

	m_pToken = &asPunctuation;
	std::advance(m_iterPos, 1);

	return success;
}

bool CLinterOperatorParser::CheckOperator() const
{
	const auto& iterPos = *m_iterPos;

	if (!iterPos->IsOperator()) {
		CLinterErrors::PushError("Expected operator, but found " + iterPos->Source(), iterPos->m_oSourcePosition);
		return false;
	}

	return true;
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