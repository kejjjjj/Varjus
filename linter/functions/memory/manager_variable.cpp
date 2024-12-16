#include "manager_variable.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"

#include <cassert>

CVariableManager::CVariableManager(class CMemory* const owner)
	: m_pOwner(owner) {
}

CLinterVariable* CVariableManager::DeclareVariable(const std::string& var) {
	assert(!var.empty());

	if (ContainsVariable(var))
		return m_oVariables[var].get();

	auto context = m_pOwner->GetContext();

	CStringIntegerHashMap& map = m_pOwner->IsGlobalMemory()
		? context->m_oAllGlobalVariables
		: context->m_oAllVariables;

	CCrossModuleReference ref(map[var]);

	m_oVariables[var] = std::make_unique<CLinterVariable>(m_pOwner, var, ref);
	return m_oVariables[var].get();
}

CLinterVariable* CVariableManager::GetVariable(const std::string& var) {
	return ContainsVariable(var) ? m_oVariables[var].get() : nullptr;
}
bool CVariableManager::ContainsVariable(const std::string& name) const {
	return m_oVariables.contains(name);
}
std::size_t CVariableManager::GetVariableCount() const noexcept {
	return m_oVariables.size();
}
