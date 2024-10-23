#include "stack.hpp"
#include "variable_declarations.hpp"
#include <cassert>

CMemory::CMemory() = default;
CMemory::~CMemory() = default;

CLinterVariable* CMemory::DeclareVariable(const std::string& var)
{
	assert(!var.empty());
	m_oVariables[var] = CLinterVariable(this, var, m_oVariables.size());
	return &m_oVariables[var];
}

CLinterVariable* CMemory::GetVariable(const std::string& var)
{
	return ContainsVariable(var) ? &m_oVariables[var] : nullptr;
}
bool CMemory::ContainsVariable(const std::string& name) const
{
	return m_oVariables.contains(name);
}

CStack::CStack() = default;
CStack::~CStack() = default;
