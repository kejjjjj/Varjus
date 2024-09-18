#include "stack.hpp"
#include "variable_declarations.hpp"
#include <cassert>

CMemoryData::CMemoryData() = default;
CMemoryData::~CMemoryData() = default;

CLinterVariable* CMemoryData::DeclareVariable(const std::string& var)
{
	assert(var.size() > 0);
	m_oVariables[var] = CLinterVariable(this, var, m_oVariables.size());
	return &m_oVariables[var];
}

CLinterVariable* CMemoryData::GetVariable(const std::string& var)
{
	return ContainsVariable(var) ? &m_oVariables[var] : nullptr;
}
bool CMemoryData::ContainsVariable(const std::string& name) const
{
	return m_oVariables.contains(name);
}

CStack::CStack() = default;
CStack::~CStack() = default;
