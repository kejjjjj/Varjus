#include "runtime/runtime.hpp"
#include "integer.hpp"

IValue* CIntValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<CIntValue>(m_oValue);
}
