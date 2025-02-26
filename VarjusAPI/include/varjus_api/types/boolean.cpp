#include "varjus_api/internal/runtime.hpp"
#include "boolean.hpp"

CBooleanValue* CBooleanValue::Construct(CProgramRuntime* const runtime, bool v) {
	return runtime->AcquireNewValue<CBooleanValue>(v);
}

IValue* CBooleanValue::Copy()
{
	if (IsShared()) {
		auto ptr = m_pAllocator->AcquireNewValue<CBooleanValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CBooleanValue::Construct(m_pAllocator, Get());
}
void CBooleanValue::Release()
{
	if (IsShared())
		ReleaseShared();
	
	ReleaseInternal();
	m_pAllocator->FreeValue<CBooleanValue>(this);
}
