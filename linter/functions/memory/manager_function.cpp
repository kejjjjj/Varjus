#include "manager_function.hpp"
#include "linter/functions/stack.hpp"
#include "linter/error.hpp"
#include "api/internal/structure.hpp"


#include <cassert>

CFunctionManager::CFunctionManager(class CMemory* const owner) 
	: m_pOwner(owner) {}


CLinterFunction* CFunctionManager::DeclareFunction(const std::string& var, std::size_t index) {
	assert(!var.empty());

	CCrossModuleReference ref(index);
	m_oFunctions[var] = std::make_unique<CLinterFunction>(var, ref);
	return m_oFunctions[var].get();
}
CLinterFunction* CFunctionManager::DeclareFunction(const CLinterFunction& func)
{
	m_oFunctions[func.m_sName] = std::make_unique<CLinterFunction>(func);
	return m_oFunctions[func.m_sName].get();
}
CLinterFunction* CFunctionManager::GetFunction(const std::string& var) {
	return ContainsFunction(var) ? m_oFunctions[var].get() : nullptr;
}
bool CFunctionManager::ContainsFunction(const std::string& name) const {
	return m_oFunctions.contains(name);
}
std::size_t CFunctionManager::GetFunctionCount() const noexcept {
	return m_oFunctions.size();
}
