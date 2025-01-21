#include "identifier.hpp"

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/functions/stack.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/context.hpp"
#include "linter/hoisting/hoisting.hpp"

#include "globalEnums.hpp"

#include <cassert>
#include <iostream>


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

	if (m_pOwner->IsHoisting() || m_pToken->Type() != TokenType::tt_name) {
		std::advance(m_iterPos, 1);
		return success;
	}

	auto scope = m_pScope.lock();

	if (!scope)
		return failure;

	auto& str = m_pToken->Source();
	if (scope->VariableExists(str)) {

#ifdef OPTIMIZATIONS
		m_pIdentifier = GetVariableByIdentifier<CConstEvalLinterVariable>(str);

		if(!m_pIdentifier) //not in the consteval list, find it from non consteval
			m_pIdentifier = GetVariableByIdentifier<CLinterVariable>(str);
#else
		m_pIdentifier = GetVariableByIdentifier<CLinterVariable>(str);
#endif
	}
	else if(ContainsFunction(str))
		m_pIdentifier = GetFunctionByIdentifier(str);

	//ignore when hoisting
	if (!m_pOwner->IsHoisting() && !m_pIdentifier) {
		CLinterErrors::PushError("Use of an undefined identifier: " + str, m_pToken->m_oSourcePosition);
		return failure;
	}
	
	std::advance(m_iterPos, 1);
	return success;
}

template CLinterVariable* CIdentifierLinter::GetVariableByIdentifier<CLinterVariable>(const std::string& str) const noexcept;
#ifdef OPTIMIZATIONS
template CLinterVariable* CIdentifierLinter::GetVariableByIdentifier<CConstEvalLinterVariable>(const std::string& str) const noexcept;
#endif

template<typename T>
[[nodiscard]] constexpr CVariableManager<T>* GetVariableManager(CMemory* memory) noexcept {
	if constexpr (std::is_same_v<T, CLinterVariable>)
		return memory->m_VariableManager.get();
	else
#ifndef OPTIMIZATIONS
		static_assert(false, "if constexpr (std::is_same_v<T, CLinterVariable>");
#else
		return memory->m_ConstEvalVariableManager.get();
#endif
}

template<typename T>
CLinterVariable* CIdentifierLinter::GetVariableByIdentifier(const std::string& str) const noexcept
{

	//find the global variable first... unlike some languages :)
	CLinterVariable* var = GetVariableManager<T>(m_pOwner->GetGlobalMemory())->GetVariable(str);

	if (var)
		return var;

	//find the variable from a local function if it exists
	if (m_pOwner->IsLocalFunction()) {
		const auto globalFunc = m_pOwner->ToStack()->GetGlobalFunction();

		var = GetVariableManager<T>(globalFunc)->GetVariable(str);

		if (var) {
			// this variable is being accessed in a lambda function -> capture it
			var->m_bCaptured = true;
			return var;
		}
	}

	return GetVariableManager<T>(m_pOwner)->GetVariable(str);
}

bool CIdentifierLinter::ContainsFunction(const std::string& str) const noexcept
{
	if (m_pOwner->HasHoistedData()) {
		auto& hoister = m_pOwner->GetHoister();
		return hoister->ContainsFunction(str);
	}
	
	return false;
}

CLinterFunction* CIdentifierLinter::GetFunctionByIdentifier(const std::string& str) const noexcept
{
	//yeaaaaahhhh this is fine
	if (m_pOwner->IsHoisting())
		return nullptr; //something is going terribly wrong if this return value is being used

	auto lowest = m_pOwner->GetGlobalMemory();
	auto& manager = lowest->m_FunctionManager;

	if (manager->ContainsFunction(str))
		return manager->GetFunction(str);

	//try seeing if this function exists in the hoisted data

	auto& hoister = m_pOwner->GetHoister();

	if (hoister->ContainsFunction(str)) {
		return manager->DeclareFunction(*hoister->GetFunctionByName(str));
	}

	assert(false);
	return nullptr;
}

bool CIdentifierLinter::CheckIdentifier(const CToken* token) const noexcept
{
	assert(token != nullptr);
	const auto type = token->Type();
	return type == TokenType::tt_name || IS_IMMEDIATE(type);
}
