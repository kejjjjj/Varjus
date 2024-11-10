#include "runtime/runtime.hpp"
#include "boolean.hpp"
#include <iostream>

IValue* CBooleanValue::Copy()
{
	return CProgramRuntime::AcquireNewValue<CBooleanValue>(m_oValue);
}
void CBooleanValue::Release()
{
	ReleaseInternal();
	CProgramRuntime::FreeValue<CBooleanValue>(this);
}
