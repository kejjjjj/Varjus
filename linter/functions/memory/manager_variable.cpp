#include "manager_variable.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"

#include <cassert>

CLinterVariable::~CLinterVariable() = default;
CLinterVariable::CLinterVariable(const CMemory* owner, const VarjusString& name, const CCrossModuleReference& ref)
	: CMemoryIdentifier(name, ref), m_pOwner(owner) {
}

template <typename T1, typename T2>
CVariableManager<T1, T2>::CVariableManager(class CMemory* const owner)
	: m_pOwner(owner) {
}

template <typename T1, typename T2>
T1* CVariableManager<T1, T2>::DeclareVariable(const VarjusString& var) {
	assert(!var.empty());

	if (ContainsVariable(var))
		return m_oVariables[var].get();

	auto context = m_pOwner->GetContext();

	CStringIntegerHashMap& map = m_pOwner->IsGlobalMemory()
		? context->m_oAllGlobalVariables
		: context->m_oAllVariables;

	CCrossModuleReference ref(map[var]);

	m_oVariables[var] = std::make_unique<T1>(m_pOwner, var, ref);

	return m_oVariables[var].get();
}

template <typename T1, typename T2>
T1* CVariableManager<T1, T2>::GetVariable(const VarjusString& var) {
	return ContainsVariable(var) ? m_oVariables[var].get() : nullptr;
}

template <typename T1, typename T2>
bool CVariableManager<T1, T2>::ContainsVariable(const VarjusString& name) const {
	return m_oVariables.contains(name);
}

template <typename T1, typename T2>
T1* CVariableManager<T1, T2>::GetVariableByIndex(std::size_t i) const
{
	for (auto& [_, v] : m_oVariables) {

		if (v->m_uIndex == i)
			return v.get();
	}

	return nullptr;

}

template <typename T1, typename T2>
std::size_t CVariableManager<T1, T2>::GetVariableCount() const noexcept {
	return m_oVariables.size();
}

template class CVariableManager<CLinterVariable>;
