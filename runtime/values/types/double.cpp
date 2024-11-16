#include "runtime/runtime.hpp"
#include "double.hpp"

#include "internal/reference.hpp"

IValue* CDoubleValue::Copy()
{
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(m_oValue);
}
void CDoubleValue::Release()
{
	CReferenceValue<CDoubleValue> dbl;

	ReleaseInternal();
	CProgramRuntime::FreeValue<CDoubleValue>(this);
}