#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/declarations/stack.hpp"
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

	if (m_pToken->Type() == TokenType::tt_name){
		if (const auto scope = m_pScope.lock()) {

			const auto varExists = scope->VariableExists(m_pToken->Source());
			const auto funcExists = m_pOwner->GetFunction(m_pToken->Source());

			if (!varExists && !funcExists) {
				CLinterErrors::PushError("Use of an undefined identifier: " + m_pToken->Source(), m_pToken->m_oSourcePosition);
				return failure;
			}

			if(varExists)
				m_pIdentifier = m_pOwner->GetVariable(m_pToken->Source());
			else if(funcExists)
				m_pIdentifier = m_pOwner->GetFunction(m_pToken->Source());
		}

		assert(m_pIdentifier != nullptr);
	}

	std::advance(m_iterPos, 1);
	return success;
}
bool CIdentifierLinter::IsImmediate() const noexcept
{
	auto type = m_pToken->Type();
	return type >= TokenType::tt_false && type <= TokenType::tt_string;
}
#pragma pack(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4062)
EValueType CIdentifierLinter::GetImmediateType() const noexcept
{
	assert(IsImmediate());

	switch (m_pToken->Type()) {
	case TokenType::tt_false:
	case TokenType::tt_true:
		return EValueType::t_boolean;
	case TokenType::tt_int:
		return EValueType::t_int;
	case TokenType::tt_double:
		return EValueType::t_double;
	case TokenType::tt_string:
		return EValueType::t_string;
	default:
		assert(false);
		return t_undefined;
	}
}
std::size_t CIdentifierLinter::GetImmediateSize() const noexcept
{
	assert(IsImmediate());

	auto type = GetImmediateType();

	switch (type) {
	case t_boolean:
		return sizeof(bool);
	case t_int:
		return sizeof(std::int64_t);
	case t_double:
		return sizeof(double);
	case t_string:
		return m_pToken->Source().size();
	default:
		assert(false);
		return 0u;
	}
}

std::string CIdentifierLinter::ToData() const noexcept
{
	std::string result;
	auto& string = m_pToken->Source();
	
	switch (GetImmediateType()) {
	case t_boolean:
		return std::string(1, m_pToken->Type() == TokenType::tt_true ? '\x01' : '\x00');
	case t_int:
		result = std::string(sizeof(std::int64_t), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<std::int64_t&>(*result.data()));
		break;
	case t_double:
		result = std::string(sizeof(double), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<double&>(*result.data()));
		break;
	case t_string:
		return string;
	}

	return result;
}

#pragma pack(pop)
bool CIdentifierLinter::CheckIdentifier(const CToken* token) const noexcept
{
	assert(token != nullptr);
	const auto type = token->Type();
	return type >= TokenType::tt_false && type <= TokenType::tt_name;
}
