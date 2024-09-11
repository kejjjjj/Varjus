#include "unary.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"

#include "globalEnums.hpp"

#include <cassert>
CUnaryLinter::CUnaryLinter(LinterIterator& pos, LinterIterator& end) : CLinter(pos, end)
{
	assert(m_iterPos != m_iterEnd);
}
CUnaryLinter::~CUnaryLinter() = default;


Success CUnaryLinter::ParseUnary()
{

	while (m_iterPos != m_iterEnd && (*m_iterPos)->IsOperator()) {
		const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(**m_iterPos);

		if (!IsUnaryOperator(asPunctuation))
			return success;
		
		m_oTokens.emplace_back(&asPunctuation);
		std::advance(m_iterPos, 1);
	}

	return success;
}
bool CUnaryLinter::IsUnaryOperator(const CPunctuationToken& token) const noexcept
{
	return token.m_ePriority == op_unary || token.m_ePriority == op_additive;
}
