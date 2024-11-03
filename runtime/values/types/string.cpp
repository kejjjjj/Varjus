#include "string.hpp"

#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* CStringValue::Copy() const
{
	return CProgramRuntime::AcquireNewStringValue(m_sValue);
}

IValue* CStringValue::Index(std::int64_t index)
{
	if (index < 0 || static_cast<size_t>(index) >= m_sValue.length())
		throw CRuntimeError("string index out of bounds");

	return CProgramRuntime::AcquireNewStringValue(std::string(size_t(1), m_sValue[static_cast<size_t>(index)]));
}