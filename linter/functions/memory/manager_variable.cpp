#include "manager_variable.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"
#include "linter/optimizations/values/types/opt_value.hpp"

#include <cassert>

CLinterVariable::~CLinterVariable() = default;
CLinterVariable::CLinterVariable(const CMemory* owner, const std::string& name, const CCrossModuleReference& ref)
	: CMemoryIdentifier(name, ref), m_pOwner(owner) {
}
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
CLinterVariable* CVariableManager::GetVariableByIndex(std::size_t i) const
{
	for (auto& [_, v] : m_oVariables) {

		if (v->m_uIndex == i)
			return v.get();
	}

	return nullptr;

}
std::size_t CVariableManager::GetVariableCount() const noexcept {
	return m_oVariables.size();
}

//#ifdef OPTIMIZATIONS
//CConstEvalLinterVariable* CVariableManager::DeclareConstEvalVariable(const std::string& var)
//{
//
//}
//#endif
//CConstEvalLinterVariable::CConstEvalLinterVariable(const CMemory* owner, const std::string& name, const CCrossModuleReference& ref)
//	: CLinterVariable(owner, name, ref){ }
//CConstEvalLinterVariable::~CConstEvalLinterVariable() = default;