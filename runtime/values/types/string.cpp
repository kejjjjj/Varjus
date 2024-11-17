#include "string.hpp"

#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* CStringValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CStringValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	return CProgramRuntime::AcquireNewValue<CStringValue>(Get());
}
void CStringValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CStringValue>(this);
}
IValue* CStringValue::Index(std::int64_t index)
{
	if (index < 0 || static_cast<size_t>(index) >= Get().length())
		throw CRuntimeError("string index out of bounds");

	auto v = CProgramRuntime::AcquireNewValue<CStringValue>(std::string(size_t(1), Get()[static_cast<size_t>(index)]));
	v->MakeImmutable(); //cannot modify parts
	return v;
}
