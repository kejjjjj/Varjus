#include "runtime/runtime.hpp"
#include "boolean.hpp"

IValue* CBooleanValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<CBooleanValue>(m_oValue);
}
