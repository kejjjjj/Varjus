#include "runtime/runtime.hpp"
#include "double.hpp"

IValue* CDoubleValue::Copy()
{
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(m_oValue);
}
void CDoubleValue::Release()
{
	ReleaseInternal();
	CProgramRuntime::FreeValue<CDoubleValue>(this);
}