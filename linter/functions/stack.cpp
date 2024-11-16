#include "stack.hpp"
#include "linter/declarations/variable_declarations.hpp"
#include "linter/functions/function.hpp"
#include "linter/context.hpp"
#include <cassert>

CMemory::CMemory(CFileRuntimeData* const file, CProgramContext* const context) 
	: m_pFile(file), m_pContext(context){
	assert(m_pFile);
	assert(m_pContext);
}
CMemory::~CMemory() = default;

CLinterVariable* CMemory::DeclareVariable(const std::string& var){
	assert(!var.empty());

	if (ContainsVariable(var))
		return &m_oVariables[var];

	m_oVariables[var] = CLinterVariable(this, var, GetContext()->m_oAllVariables[var]);
	return &m_oVariables[var];
}

CLinterVariable* CMemory::GetVariable(const std::string& var){
	return ContainsVariable(var) ? &m_oVariables[var] : nullptr;
}
bool CMemory::ContainsVariable(const std::string& name) const{
	return m_oVariables.contains(name);
}
std::size_t CMemory::GetVariableCount() const noexcept{
	return m_oVariables.size();
}

/***********************************************************************
 > 
***********************************************************************/

CLinterFunction* CMemory::DeclareFunction(const std::string& var) {
	assert(!var.empty());
	m_oFunctions[var] = CLinterFunction(var, GetFunctionIndex(var));
	return &m_oFunctions[var];
}
CLinterFunction* CMemory::GetFunction(const std::string& var){
	return ContainsFunction(var) ? &m_oFunctions[var] : nullptr;
}
std::size_t CMemory::GetFunctionIndex(const std::string& n) {
	assert(ContainsFunctionGlobally(n));
	return m_pFile->FindFunctionIndex(n);
}
bool CMemory::ContainsFunction(const std::string& name) const {
	return m_oFunctions.contains(name);
}
bool CMemory::ContainsFunctionGlobally(const std::string& name) const {
	return !!m_pFile->FindFunction(name);
}
std::size_t CMemory::GetFunctionCount() const noexcept {
	return m_oFunctions.size();
}

CStack* CMemory::ToStack() { return dynamic_cast<CStack*>(this); }
auto CMemory::ToStack() const { return dynamic_cast<const CStack*>(this); }

CStack::CStack(CFileRuntimeData* const file, CProgramContext* const context) 
	: CMemory(file, context) {};
CStack::CStack(std::unique_ptr<CFunctionBlock>&& func, CFileRuntimeData* const file, CProgramContext* const context)
	: CMemory(file, context), m_pFunction(std::move(func)){}
CStack::~CStack() = default;

void CStack::AddFunctionInstruction(RuntimeBlock&& block) const
{
	m_pFunction->m_oInstructions.emplace_back(std::move(block));
}

CStack* CStack::GetGlobalFunction()
{
	auto func = this;

	while (func->m_pLowerFunction) {
		func = func->m_pLowerFunction;
	}

	return func;
}
