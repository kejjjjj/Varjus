#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include "linter/functions/function.hpp"

#include <ranges>
#include <algorithm>

#include <chrono>

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction) :
	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->GetVariableCount())
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

bool CRuntimeFunction::Execute([[maybe_unused]] CFunction* const thisFunction)
{
	std::chrono::time_point<std::chrono::steady_clock> old = std::chrono::steady_clock::now();

	auto& variablePool = CProgramRuntime::m_oVariablePool;
	auto func = CFunction(variablePool.Acquire(m_uNumVariables));

	bool returnVal = false;

	for (const auto& insn : m_oInstructions) {
		if (insn->Execute(&func)) {
			returnVal = true;
		}
	}

	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	std::chrono::duration<float> difference = now - old;

	std::ranges::for_each(func.m_oStack, [&variablePool](std::unique_ptr<CVariable>& v) {
		v->GetValue()->Print();
		v->GetValue()->Release();
		variablePool.Release(std::move(v)); });

	printf("\ntime taken: %.6f\n", difference.count());

	return returnVal;
}

CFunction::CFunction(VectorOf<std::unique_ptr<CVariable>>&& variables) : m_oStack(std::move(variables))
{
	for (auto& v : m_oStack)
		v->SetValue(CProgramRuntime::AcquireNewValue());
}

CVariable* CFunction::GetVariableByIndex(std::size_t index) const
{
	return m_oStack[index].get();
}