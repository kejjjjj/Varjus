#include "runtime/runtime.hpp"
#include "boolean.hpp"

IValue* CBooleanValue::Copy() const
{
	return CProgramRuntime::AcquireNewBooleanValue(m_bValue);
}
