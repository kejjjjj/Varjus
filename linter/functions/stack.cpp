#include "stack.hpp"
#include "linter/declarations/variable_declarations.hpp"
#include "linter/functions/function.hpp"
#include "linter/context.hpp"
#include "linter/modules/module.hpp"

#include <cassert>

using namespace Varjus;

bool CLinterVariable::IsGlobal() const noexcept {
	assert(m_pOwner);
	return m_pOwner == m_pOwner->GetGlobalMemory();
}

CMemory::CMemory(CProgramInformation* const program, CMemory* globalMemory, CModule* const file)
	: m_pModule(file), m_pGlobal(globalMemory), m_pProgram(program) {
	assert(m_pModule);

	if (!m_pGlobal)
		m_pGlobal = this;

	m_VariableManager = std::make_unique<CVariableManager<CLinterVariable>>(this);
	m_FunctionManager = std::make_unique<CFunctionManager>(this);
}
CMemory::~CMemory() = default;

CFileContext* CMemory::GetContext() const {
	return m_pModule->GetContext();
}
CStack* CMemory::ToStack() { return dynamic_cast<CStack*>(this); }
auto CMemory::ToStack() const { return dynamic_cast<const CStack*>(this); }

CStack::CStack(CProgramInformation* const program, CMemory* globalMemory, CModule* const file)
	: CMemory(program, globalMemory, file) {};

CStack::CStack(CProgramInformation* const program, CMemory* globalMemory, std::unique_ptr<CFunctionBlock>&& func, CModule* const file) 
	: CMemory(program, globalMemory, file), m_pFunction(std::move(func)){}

CStack::~CStack() = default;

void CStack::AddFunctionInstruction(__RuntimeBlock&& block) const{
	m_pFunction->m_oInstructions.emplace_back(std::move(block));
}
const VectorOf<__RuntimeBlock>& CStack::GetInstructions() const noexcept
{
	assert(m_pFunction);
	return m_pFunction->m_oInstructions;
}
CStack* CStack::GetGlobalFunction()
{
	auto func = this;

	while (func->m_pLowerFunction) {
		func = func->m_pLowerFunction;
		assert(func);
	}

	return func;
}
