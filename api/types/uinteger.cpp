#include "runtime/runtime.hpp"
#include "uinteger.hpp"

CUIntValue* CUIntValue::Construct(VarjusUInt v) {
	return CProgramRuntime::AcquireNewValue<CUIntValue>(v);
}

IValue* CUIntValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CUIntValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CProgramRuntime::AcquireNewValue<CUIntValue>(Get());
}
void CUIntValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CUIntValue>(this);
}