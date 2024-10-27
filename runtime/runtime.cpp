#include "runtime.hpp"
#include "structure.hpp"
#include "values/simple.hpp"
#include "values/integer.hpp"
#include "values/double.hpp"

#include "variables.hpp"

#include <ranges>

CNonOwningObjectPool<CVariable> CProgramRuntime::m_oVariablePool(100);

COwningObjectPool<IValue>		CProgramRuntime::m_oUndefinedValuePool(100);
COwningObjectPool<CIntValue>	CProgramRuntime::m_oIntValuePool(100);
COwningObjectPool<CDoubleValue> CProgramRuntime::m_oDoubleValuePool(100);

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

	if ((*iMainFunction)->Execute(nullptr))
		return;

	return;

}
IValue* CProgramRuntime::AcquireNewValue(){
	return m_oUndefinedValuePool.acquire();
}
CIntValue* CProgramRuntime::AcquireNewIntValue(){
	return m_oIntValuePool.acquire();
}
CDoubleValue* CProgramRuntime::AcquireNewDoubleValue(){
	return m_oDoubleValuePool.acquire();
}

void CProgramRuntime::FreeUndefinedValue(IValue* value) {
	return m_oUndefinedValuePool.release(value);
}
void CProgramRuntime::FreeIntValue(CIntValue* value) {
	return m_oIntValuePool.release(value);
}
void CProgramRuntime::FreeDoubleValue(CDoubleValue* value){
	return m_oDoubleValuePool.release(value);
}
