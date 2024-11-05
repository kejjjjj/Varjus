#include "runtime/runtime.hpp"
#include "double.hpp"

IValue* CDoubleValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(m_oValue);
}
