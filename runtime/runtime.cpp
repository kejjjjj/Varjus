#include "runtime.hpp"
#include "structure.hpp"
#include "values/types/types.hpp"
#include "variables.hpp"

#include <ranges>
#include <iostream>

CNonOwningObjectPool<CVariable>  CProgramRuntime::m_oVariablePool(100);

COwningObjectPool<IValue>        CProgramRuntime::m_oUndefinedValuePool(100);
COwningObjectPool<CBooleanValue> CProgramRuntime::m_oBooleanValuePool(100);
COwningObjectPool<CIntValue>     CProgramRuntime::m_oIntValuePool(100);
COwningObjectPool<CDoubleValue>  CProgramRuntime::m_oDoubleValuePool(100);
COwningObjectPool<CStringValue>  CProgramRuntime::m_oStringValuePool(100);

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

	if ((*iMainFunction)->Execute(nullptr)) {
	}

	std::cout << "\n\n--------------LEAKS--------------\n\n";
	std::cout << std::format("undefined: {}\n", m_oUndefinedValuePool.GetInUseCount());
	std::cout << std::format("boolean:   {}\n", m_oBooleanValuePool.GetInUseCount());
	std::cout << std::format("int:       {}\n", m_oIntValuePool.GetInUseCount());
	std::cout << std::format("double:    {}\n", m_oDoubleValuePool.GetInUseCount());
	std::cout << std::format("string:    {}\n\n", m_oStringValuePool.GetInUseCount());


	return;

}
IValue* CProgramRuntime::AcquireNewValue(){
	return m_oUndefinedValuePool.Acquire();
}
CBooleanValue* CProgramRuntime::AcquireNewBooleanValue(bool value)
{
	auto v = m_oBooleanValuePool.Acquire();
	v->m_bValue = value;
	return v;
}
CIntValue* CProgramRuntime::AcquireNewIntValue(std::int64_t value){
	auto v = m_oIntValuePool.Acquire();
	v->m_iValue = value;
	return v;
}
CDoubleValue* CProgramRuntime::AcquireNewDoubleValue(double value){
	auto v = m_oDoubleValuePool.Acquire();
	v->m_dValue = value;
	return v;
}
CStringValue* CProgramRuntime::AcquireNewStringValue(const std::string& value)
{
	auto v = m_oStringValuePool.Acquire();
	v->m_sValue = value;
	return v;
}
void CProgramRuntime::FreeUndefinedValue(IValue* value) {
	return m_oUndefinedValuePool.Release(value);
}
void CProgramRuntime::FreeBooleanValue(CBooleanValue* value){
	return m_oBooleanValuePool.Release(value);
}
void CProgramRuntime::FreeIntValue(CIntValue* value) {
	return m_oIntValuePool.Release(value);
}
void CProgramRuntime::FreeDoubleValue(CDoubleValue* value){
	return m_oDoubleValuePool.Release(value);
}
void CProgramRuntime::FreeStringValue(CStringValue* value) {
	return m_oStringValuePool.Release(value);
}