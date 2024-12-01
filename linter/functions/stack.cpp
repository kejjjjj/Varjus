#include "stack.hpp"
#include "linter/declarations/variable_declarations.hpp"
#include "linter/functions/function.hpp"
#include "linter/context.hpp"
#include <cassert>


bool CLinterVariable::IsGlobal() const noexcept {
	assert(m_pOwner);
	return m_pOwner == m_pOwner->GetGlobalMemory();
}

CMemory::CMemory(CMemory* globalMemory, CFileRuntimeData* const file, CProgramContext* const context)
	: m_pFile(file), m_pContext(context), m_pGlobal(globalMemory) {
	assert(m_pFile);
	assert(m_pContext);

	if (!m_pGlobal)
		m_pGlobal = this;

	m_VariableManager = std::make_unique<CVariableManager>(this);
	m_FunctionManager = std::make_unique<CFunctionManager>(this);
}
CMemory::~CMemory() = default;

CStack* CMemory::ToStack() { return dynamic_cast<CStack*>(this); }
auto CMemory::ToStack() const { return dynamic_cast<const CStack*>(this); }

CStack::CStack(CMemory* globalMemory, CFileRuntimeData* const file, CProgramContext* const context)
	: CMemory(globalMemory, file, context) {};

CStack::CStack(CMemory* globalMemory, std::unique_ptr<CFunctionBlock>&& func, 
	CFileRuntimeData* const file, CProgramContext* const context)
	: CMemory(globalMemory, file, context), m_pFunction(std::move(func)){}

CStack::~CStack() = default;

void CStack::AddFunctionInstruction(RuntimeBlock&& block) const{
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
