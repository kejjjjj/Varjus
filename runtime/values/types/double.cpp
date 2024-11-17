#include "runtime/runtime.hpp"
#include "double.hpp"

IValue* CDoubleValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CDoubleValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CProgramRuntime::AcquireNewValue<CDoubleValue>(Get());
}
void CDoubleValue::Release()
{
	if (IsShared())
		ReleaseShared();
	ReleaseInternal();
	CProgramRuntime::FreeValue<CDoubleValue>(this);
}