#include "varjus_api/internal/runtime.hpp"
#include "double.hpp"

CDoubleValue* CDoubleValue::Construct(Varjus::CProgramRuntime* const runtime, VarjusDouble v) {
	return runtime->AcquireNewValue<CDoubleValue>(v);
}

IValue* CDoubleValue::Copy()
{
	if (IsShared()) {
		auto ptr = m_pAllocator->AcquireNewValue<CDoubleValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return m_pAllocator->AcquireNewValue<CDoubleValue>(Get());
}
void CDoubleValue::Release()
{
	if (IsShared())
		ReleaseShared();
	ReleaseInternal();
	m_pAllocator->FreeValue<CDoubleValue>(this);
}