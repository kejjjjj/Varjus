#include "runtime/runtime.hpp"
#include "string.hpp"

IValue* CStringValue::Copy() const
{
	return CProgramRuntime::AcquireNewStringValue(m_sValue);
}