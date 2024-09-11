#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"

#include "globalEnums.hpp"

#include <cassert>


CIdentifierLinter::CIdentifierLinter(LinterIterator& pos, LinterIterator& end) : CLinter(pos, end)
{
	assert(m_iterPos != m_iterEnd);
}
CIdentifierLinter::~CIdentifierLinter() = default;

[[nodiscard]] Success CIdentifierLinter::ParseIdentifier()
{
	if (m_iterPos == m_iterEnd) {
		CLinterErrors::PushError("Expected identifier, but reached end of file", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	auto& iterPos = *m_iterPos;

	if (iterPos->Type() != TokenType::t_name) {
		CLinterErrors::PushError("Expected identifier, but found " + iterPos->Source(), iterPos->m_oSourcePosition);
		return failure;
	}

	m_oTokens.push_back(&**m_iterPos);

	std::advance(m_iterPos, 1);
	return success;
}
