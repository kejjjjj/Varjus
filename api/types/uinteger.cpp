#include "api/internal/runtime.hpp"
#include "uinteger.hpp"

CUIntValue* CUIntValue::Construct(CProgramRuntime* const runtime, VarjusUInt v) {
	return runtime->AcquireNewValue<CUIntValue>(v);
}

IValue* CUIntValue::Copy()
{
	if (IsShared()) {
		auto ptr = m_pAllocator->AcquireNewValue<CUIntValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}
	return m_pAllocator->AcquireNewValue<CUIntValue>(Get());
}
void CUIntValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();
	m_pAllocator->FreeValue<CUIntValue>(this);
}