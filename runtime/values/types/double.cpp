#include "runtime/runtime.hpp"
#include "double.hpp"

IValue* CDoubleValue::Copy() const
{
	return CProgramRuntime::AcquireNewDoubleValue(m_dValue);
}
