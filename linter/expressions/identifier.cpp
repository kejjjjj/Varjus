#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/functions/stack.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/context.hpp"

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

	if (m_pToken->Type() != TokenType::tt_name) {
		std::advance(m_iterPos, 1);
		return success;
	}

	auto scope = m_pScope.lock();

	if (!scope)
		return failure;

	auto& str = m_pToken->Source();
	if(scope->VariableExists(str))
		m_pIdentifier = GetVariableByIdentifier(str);
	else if(m_pOwner->m_FunctionManager->ContainsFunctionGlobally(str))
		m_pIdentifier = GetFunctionByIdentifier(str);

	if (!m_pIdentifier) {
		CLinterErrors::PushError("Use of an undefined identifier: " + str, m_pToken->m_oSourcePosition);
		return failure;
	}
	
	std::advance(m_iterPos, 1);
	return success;
}
CLinterVariable* CIdentifierLinter::GetVariableByIdentifier(const std::string& str) const noexcept
{
	//find the global variable first... unlike some languages :)
	auto var = m_pOwner->GetGlobalMemory()->m_VariableManager->GetVariable(str);

	if (var)
		return var;

	var = m_pOwner->m_VariableManager->GetVariable(str);
	if (!var) {

		assert(m_pOwner->IsStack());
		auto stack = m_pOwner->ToStack();
		const auto globalFunc = stack->GetGlobalFunction();
		assert(globalFunc);
		var = globalFunc->m_VariableManager->GetVariable(str);
		assert(var);

		// this variable is being accessed in a lambda function
		stack->AddSharedOwnershipVariable(var->m_uIndex);
	}

	return var;
}
CLinterFunction* CIdentifierLinter::GetFunctionByIdentifier(const std::string& str) const noexcept
{
	auto lowest = m_pOwner->GetGlobalMemory();
	auto& manager = lowest->m_FunctionManager;

	return manager->ContainsFunction(str)
		? manager->GetFunction(str)
		: manager->DeclareFunction(str);
}

bool CIdentifierLinter::CheckIdentifier(const CToken* token) const noexcept
{
	assert(token != nullptr);
	const auto type = token->Type();
	return type == TokenType::tt_name || IS_IMMEDIATE(type);
}
