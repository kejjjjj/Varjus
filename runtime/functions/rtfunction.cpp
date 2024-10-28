#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"

#include "runtime/values/simple.hpp"

#include <ranges>
#include <algorithm>

bool CRuntimeFunction::Execute([[maybe_unused]] CFunction* const thisFunction)
{
	auto& variablePool = CProgramRuntime::m_oVariablePool;
	auto func = CFunction(variablePool.acquire(m_uNumVariables));

	bool returnVal = false;

	for (const auto& insn : m_oInstructions) {
		if (insn->Execute(&func)) {
			returnVal = true;
		}
	}

	std::ranges::for_each(func.m_oStack, [&variablePool](std::unique_ptr<CVariable>& v) {
		v->GetValue()->Print();
		variablePool.release(std::move(v)); });

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