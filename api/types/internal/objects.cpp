#include "objects.hpp"
#include "object_declarations.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/structure.hpp"


#include "linter/context.hpp"
#include <iostream>

CBuiltInObject::CBuiltInObject() : CObjectValue() {}
CBuiltInObject::~CBuiltInObject() = default;

CBuiltInObject* CBuiltInObject::Construct(BuiltInMethod_t&& methods, BuiltInProperty_t&& properties)
{
	auto ptr = CProgramRuntime::AcquireNewValue<CBuiltInObject>();
	ptr->MakeShared();
	ptr->m_oMethods = std::make_shared<BuiltInMethod_t>(std::move(methods));
	ptr->m_oProperties = std::make_shared<BuiltInProperty_t>(std::move(properties));
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
	ptr->m_oProperties = m_oProperties;
	return ptr;
}

IValue* CBuiltInObject::GetAggregate(std::size_t memberIdx) {

	if (m_oMethods->contains(memberIdx)) {
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(this);
	}

	return CObjectValue::GetAggregate(memberIdx);
}

CBuiltInObjectPairs::~CBuiltInObjectPairs() = default;


std::vector<std::pair<std::string, CBuiltInObjectPairs>> CBuiltInObjects::m_arrData;
void CBuiltInObjects::AddNewGlobalObject(const std::string& name,
	const OptionalCtor<BuiltInMethod_t>& createMethods,
	const OptionalCtor<BuiltInProperty_t>& createProperties)
{
	m_arrData.push_back({ name, { createMethods, createProperties} });
}