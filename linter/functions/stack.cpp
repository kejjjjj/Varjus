#include "stack.hpp"
#include "linter/declarations/variable_declarations.hpp"
#include "linter/functions/function.hpp"
#include "linter/context.hpp"
#include "linter/modules/module.hpp"

#include <cassert>


bool CLinterVariable::IsGlobal() const noexcept {
	assert(m_pOwner);
	return m_pOwner == m_pOwner->GetGlobalMemory();
}

#ifdef OPTIMIZATIONS
#include "linter/optimizations/optimizations.hpp"
void CLinterVariable::MakeConstEval()
{
	assert(!IsConstEval());

	m_pConstEval = COptimizationValues::AcquireNewVariable();
	m_pConstEval->SetValue(COptimizationValues::AcquireNewValue<IConstEvalValue>());

}
void CLinterVariable::ReleaseConstEval()
{
	assert(IsConstEval());
	m_pConstEval->Release();
}
#endif

CMemory::CMemory(CMemory* globalMemory, CModule* const file)
	: m_pModule(file), m_pGlobal(globalMemory) {
	assert(m_pModule);

	if (!m_pGlobal)
		m_pGlobal = this;

	m_VariableManager = std::make_unique<CVariableManager>(this);
	m_FunctionManager = std::make_unique<CFunctionManager>(this);
}
CMemory::~CMemory() = default;

CFileContext* CMemory::GetContext() const {
	return m_pModule->GetContext();
}
CStack* CMemory::ToStack() { return dynamic_cast<CStack*>(this); }
auto CMemory::ToStack() const { return dynamic_cast<const CStack*>(this); }

CStack::CStack(CMemory* globalMemory, CModule* const file)
	: CMemory(globalMemory, file) {};

CStack::CStack(CMemory* globalMemory, std::unique_ptr<CFunctionBlock>&& func, 
	CModule* const file) : CMemory(globalMemory, file), 
	m_pFunction(std::move(func)){}

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
