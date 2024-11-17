#include "runtime/runtime.hpp"
#include "boolean.hpp"

IValue* CBooleanValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CBooleanValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(Get());
}
void CBooleanValue::Release()
{
	if (IsShared())
		ReleaseShared();
	
	ReleaseInternal();
	CProgramRuntime::FreeValue<CBooleanValue>(this);
}
