#include "api/internal/runtime.hpp"
#include "integer.hpp"

CIntValue* CIntValue::Construct(CProgramRuntime* const runtime, VarjusInt v) {
	return runtime->AcquireNewValue<CIntValue>(v);
}

IValue* CIntValue::Copy()
{
	if (IsShared()) {
		auto ptr = m_pAllocator->AcquireNewValue<CIntValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return m_pAllocator->AcquireNewValue<CIntValue>(Get());
}
void CIntValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();
	m_pAllocator->FreeValue<CIntValue>(this);
}