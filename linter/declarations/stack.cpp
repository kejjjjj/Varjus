#include "stack.hpp"
#include "variable_declarations.hpp"
#include "linter/functions/function.hpp"

#include <cassert>

CMemory::CMemory(CFileRuntimeData* const file) : m_pFile(file){}
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
std::size_t CMemory::GetVariableCount() const noexcept
{
	return m_oVariables.size();
}
CStack* CMemory::ToStack() { return dynamic_cast<CStack*>(this); }
auto CMemory::ToStack() const { return dynamic_cast<const CStack*>(this); }

CStack::CStack(CFileRuntimeData* const file) : CMemory(file) {};
CStack::CStack(std::unique_ptr<CFunctionBlock>&& func, CFileRuntimeData* const file)
	: CMemory(file), m_pFunction(std::move(func)){}
CStack::~CStack() = default;

void CStack::AddFunctionInstruction(RuntimeBlock&& block) const
{
	m_pFunction->m_oInstructions.emplace_back(std::move(block));
}
