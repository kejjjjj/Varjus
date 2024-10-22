#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/declarations/stack.hpp"


#include "globalEnums.hpp"

#include <cassert>


CIdentifierLinter::CIdentifierLinter(LinterIterator& pos, LinterIterator& end, CMemory* const stack) 
	: CLinterSingle(pos, end), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CIdentifierLinter::~CIdentifierLinter() = default;

Success CIdentifierLinter::ParseIdentifier()
{
	if (m_iterPos == m_iterEnd) {
		CLinterErrors::PushError("Expected identifier, but reached end of file", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	auto& iterPos = *m_iterPos;

	if (!CheckIdentifier(iterPos)) {
		CLinterErrors::PushError("Expected identifier, but found " + iterPos->Source(), iterPos->m_oSourcePosition);
		return failure;
	}

	m_pToken = &*iterPos;

	if (m_pToken->Type() == TokenType::t_name)
	{
		if (!m_pOwner->ContainsVariable(m_pToken->Source())) {
			CLinterErrors::PushError("Use of an undefined variable: " + m_pToken->Source(), m_pToken->m_oSourcePosition);
			return failure;
		}

		m_pVariable = m_pOwner->GetVariable(m_pToken->Source());
	}

	std::advance(m_iterPos, 1);
	return success;
}
bool CIdentifierLinter::CheckIdentifier(const CToken* token) const noexcept
{
	assert(token != nullptr);
	const auto type = token->Type();
	return type >= TokenType::t_int && type <= TokenType::t_name;
}


std::int64_t CIdentifierLinter::ToInt() const noexcept
{
	assert(m_pToken->Type() == TokenType::t_int);
	return std::stoll(m_pToken->Source());
}
