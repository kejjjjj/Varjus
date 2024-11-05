#include "runtime.hpp"
#include "structure.hpp"
#include "values/types/types.hpp"
#include "variables.hpp"

#include <ranges>
#include <iostream>

CNonOwningObjectPool<CVariable>  CProgramRuntime::m_oVariablePool(100);

template<> COwningObjectPool<IValue>        CProgramRuntime::m_oValuePool<IValue>(100);
template<> COwningObjectPool<CBooleanValue> CProgramRuntime::m_oValuePool<CBooleanValue>(100);
template<> COwningObjectPool<CIntValue>     CProgramRuntime::m_oValuePool<CIntValue>(100);
template<> COwningObjectPool<CDoubleValue>  CProgramRuntime::m_oValuePool<CDoubleValue>(100);
template<> COwningObjectPool<CStringValue>  CProgramRuntime::m_oValuePool<CStringValue>(100);

CProgramRuntime::CProgramRuntime(CFileRuntimeData* const file) :
	m_oFunctions(std::move(file->m_oFunctions))
{

}
CProgramRuntime::~CProgramRuntime() = default;


void CProgramRuntime::Execute()
{

	const auto iMainFunction = std::ranges::find(m_oFunctions, "main", [](const RuntimeFunction& rf) { return rf->GetName(); });

	if (iMainFunction == m_oFunctions.end()) {
		return;
	}

	std::vector<IValue*> args;
	if ((*iMainFunction)->Execute(nullptr, args)) {
	}

	std::cout << "\n\n--------------LEAKS--------------\n\n";
	std::cout << std::format("undefined: {}\n",   GetPool<IValue>().GetInUseCount());
	std::cout << std::format("boolean:   {}\n",   GetPool<CBooleanValue>().GetInUseCount());
	std::cout << std::format("int:       {}\n",   GetPool<CIntValue>().GetInUseCount());
	std::cout << std::format("double:    {}\n",   GetPool<CDoubleValue>().GetInUseCount());
	std::cout << std::format("string:    {}\n\n", GetPool<CStringValue>().GetInUseCount());


	return;

}