#include "runtime/runtime.hpp"
#include "integer.hpp"

IValue* CIntValue::Copy() const
{
	return CProgramRuntime::AcquireNewIntValue(m_iValue);
}
