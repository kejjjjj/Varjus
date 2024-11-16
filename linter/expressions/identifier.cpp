#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/functions/stack.hpp"
#include "linter/scopes/scope.hpp"

#include "globalEnums.hpp"

#include <cassert>


CIdentifierLinter::CIdentifierLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CIdentifierLinter::~CIdentifierLinter() = default;

Success CIdentifierLinter::ParseIdentifier()
{
	auto iterPos = GetIteratorSafe();

	if (IsEndOfBuffer() || !CheckIdentifier(iterPos)) {
		CLinterErrors::PushError("expected an identifier, but found: " + iterPos->Source(), iterPos->m_oSourcePosition);
		return failure;
	}

	m_pToken = iterPos;

	if (m_pToken->Type() == TokenType::tt_name){
		if (const auto scope = m_pScope.lock()) {

			auto& str = m_pToken->Source();

			const auto varExists = scope->VariableExists(str);
			const auto func = m_pOwner->ContainsFunctionGlobally(str);

			if (!varExists && !func) {
				CLinterErrors::PushError("Use of an undefined identifier: " + str, m_pToken->m_oSourcePosition);
				return failure;
			}

			if(varExists)
				m_pIdentifier = m_pOwner->GetVariable(str);
			else if (func) {
				m_pIdentifier = m_pOwner->ContainsFunction(str) 
					? m_pOwner->GetFunction(str) 
					: m_pOwner->DeclareFunction(str);
			}

		}

		assert(m_pIdentifier != nullptr);
	}

	std::advance(m_iterPos, 1);
	return success;
}

bool CIdentifierLinter::CheckIdentifier(const CToken* token) const noexcept
{
	assert(token != nullptr);
	const auto type = token->Type();
	return type == TokenType::tt_name || IS_IMMEDIATE(type);
}
