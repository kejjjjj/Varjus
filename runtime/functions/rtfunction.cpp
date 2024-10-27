#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"

#include <ranges>
#include <algorithm>

bool CRuntimeFunction::Execute([[maybe_unused]] CFunction* const thisFunction)
{
	auto& variablePool = CProgramRuntime::m_oVariablePool;
	auto func = CFunction(variablePool.acquire(m_uNumVariables));

	for (const auto& insn : m_oInstructions) {
		if (insn->Execute(&func)) {
			return true;
		}
	}

	std::ranges::for_each(func.m_oStack, [&variablePool](std::unique_ptr<CVariable>& v) {
		variablePool.release(std::move(v)); });

	return false;
}

CFunction::CFunction(VectorOf<std::unique_ptr<CVariable>>&& variables) : m_oStack(std::move(variables)){}

CVariable* CFunction::GetVariableByIndex(std::size_t index) const
{
	return m_oStack[index].get();
}