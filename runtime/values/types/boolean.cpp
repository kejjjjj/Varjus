#include "runtime/runtime.hpp"
#include "boolean.hpp"

IValue* CBooleanValue::Copy()
{
	return CProgramRuntime::AcquireNewValue<CBooleanValue>(m_oValue);
}
void CBooleanValue::Release()
{
	ReleaseInternal();
	CProgramRuntime::FreeValue<CBooleanValue>(this);
}
