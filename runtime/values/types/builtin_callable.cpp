#include "builtin_callable.hpp"
#include "runtime/runtime.hpp"

IValue* CMemberCallableValue::Copy() {

	auto ptr = CProgramRuntime::AcquireNewValue<CMemberCallableValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();

	return ptr;

}

void CMemberCallableValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CMemberCallableValue>(this);
	ReleaseShared();

}


void CMemberFunctionContent::Release() {
	m_pThis->Release();
}
