#include "string.hpp"

#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* CStringValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<CStringValue>(m_oValue);
}
void CStringValue::Release()
{
	ReleaseInternal();
	CProgramRuntime::FreeValue<CStringValue>(this);
}
IValue* CStringValue::Index(std::int64_t index)
{
	if (index < 0 || static_cast<size_t>(index) >= m_oValue.length())
		throw CRuntimeError("string index out of bounds");

	return CProgramRuntime::AcquireNewValue<CStringValue>(std::string(size_t(1), m_oValue[static_cast<size_t>(index)]));
}
