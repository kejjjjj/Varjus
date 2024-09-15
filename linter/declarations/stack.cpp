#include "stack.hpp"
#include "variable_declarations.hpp"

CMemoryData::CMemoryData() = default;
CMemoryData::~CMemoryData() = default;

void CMemoryData::DeclareVariable(const std::string& var)
{
	m_oVariables.push_back(var);
}
bool CMemoryData::ContainsVariable(const std::string_view& name) const
{
	return std::find(m_oVariables.begin(), m_oVariables.end(), name) != m_oVariables.end();
}

CStack::CStack() = default;
CStack::~CStack() = default;
