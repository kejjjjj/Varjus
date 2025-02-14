#include "internal_objects.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"
#include <iostream>

CBuiltInObject::CBuiltInObject() : CObjectValue() {}
CBuiltInObject::~CBuiltInObject() = default;

CBuiltInObject* CBuiltInObject::Construct([[maybe_unused]] BuiltInMethod_t&& methods)
{
	auto ptr = CProgramRuntime::AcquireNewValue<CBuiltInObject>();
	ptr->MakeShared();
	ptr->m_oMethods = std::make_shared<BuiltInMethod_t>(std::move(methods));
	return ptr;
}

void CBuiltInObject::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CBuiltInObject>(this);
	ReleaseShared();
}

IValue* CBuiltInObject::Copy() {

	CBuiltInObject* ptr = CProgramRuntime::AcquireNewValue<CBuiltInObject>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();
	ptr->m_oMethods = m_oMethods;
	return ptr;
}

IValue* CBuiltInObject::GetAggregate(std::size_t memberIdx) {

	if (m_oMethods->contains(memberIdx)) {
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		METHOD_BIND_PTR(v, this->Copy());
		return v;
	}

	return CObjectValue::GetAggregate(memberIdx);
}


std::vector<std::pair<std::string, std::function<BuiltInMethod_t()>>> CBuiltInObjects::m_arrMethods;
void CBuiltInObjects::AddNewGlobalObject(const std::string& name, const std::function<BuiltInMethod_t()>& createMethods)
{
	m_arrMethods.push_back({ name, createMethods });
}