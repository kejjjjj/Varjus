#include "runtime/runtime.hpp"
#include "callable.hpp"

IValue* CCallableValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<CCallableValue>(m_oValue);
}
void CCallableValue::Release()
{
	ReleaseInternal();
	CProgramRuntime::FreeValue<CCallableValue>(this);
}
