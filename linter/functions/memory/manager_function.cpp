#include "manager_function.hpp"
#include "linter/functions/stack.hpp"
#include "runtime/structure.hpp"

#include <cassert>

CFunctionManager::CFunctionManager(class CMemory* const owner) 
	: m_pOwner(owner) {}


CLinterFunction* CFunctionManager::DeclareFunction(const std::string& var) {
	assert(!var.empty());
	m_oFunctions[var] = CLinterFunction(var, GetFunctionIndex(var));
	return &m_oFunctions[var];
}
CLinterFunction* CFunctionManager::GetFunction(const std::string& var) {
	return ContainsFunction(var) ? &m_oFunctions[var] : nullptr;
}
std::size_t CFunctionManager::GetFunctionIndex(const std::string& n) {
	assert(ContainsFunctionGlobally(n));
	return m_pOwner->m_pFile->FindFunctionIndex(n);
}
bool CFunctionManager::ContainsFunction(const std::string& name) const {
	return m_oFunctions.contains(name);
}
bool CFunctionManager::ContainsFunctionGlobally(const std::string& name) const {
	return !!m_pOwner->m_pFile->FindFunction(name);
}
std::size_t CFunctionManager::GetFunctionCount() const noexcept {
	return m_oFunctions.size();
}
