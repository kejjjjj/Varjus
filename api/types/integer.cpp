#include "runtime/runtime.hpp"
#include "integer.hpp"

CIntValue* CIntValue::Construct(std::int64_t v) {
	return CProgramRuntime::AcquireNewValue<CIntValue>(v);
}

IValue* CIntValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CIntValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CProgramRuntime::AcquireNewValue<CIntValue>(Get());
}
void CIntValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CIntValue>(this);
}