#include "runtime.hpp"
#include <ranges>

CProgramRuntime::CProgramRuntime(CFileRuntimeData* const file) :
	m_oFunctions(std::move(file->m_oFunctions)) {
}
CProgramRuntime::~CProgramRuntime() = default;


void CProgramRuntime::Execute()
{
	const auto mainFunction = std::ranges::find(m_oFunctions, "main", [](const RuntimeFunction& rf) { return rf->GetName(); });

	if (mainFunction == m_oFunctions.end()) {
		return;
	}

	return;

}
