#include "builtin_callable.hpp"
#include "runtime/runtime.hpp"

IValue* CBuiltInMemberCallableValue::Copy() {

	std::cout << "CBuiltInMemberCallableValue: COPY" << '\n';


	auto ptr = CProgramRuntime::AcquireNewValue<CBuiltInMemberCallableValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();

	return ptr;

}

void CBuiltInMemberCallableValue::Release() {

	std::cout << "CBuiltInMemberCallableValue: " << SharedRefCount() << '\n';

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CBuiltInMemberCallableValue>(this);
	ReleaseShared();

}


void CBuiltInMemberFunctionContent::Release() {
	
	if (!m_pThis || m_pThis->HasOwner())
		return;

	auto temp = m_pThis;
	m_pThis = nullptr;
	temp->Release();
}
