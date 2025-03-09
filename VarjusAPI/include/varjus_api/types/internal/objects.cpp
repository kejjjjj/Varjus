#include "objects.hpp"
#include "object_declarations.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"


#include "linter/context.hpp"
#include <iostream>

CBuiltInObject::CBuiltInObject() : CObjectValue() {}
CBuiltInObject::~CBuiltInObject() = default;

CBuiltInObject* CBuiltInObject::Construct(CProgramRuntime* const runtime, BuiltInMethod_t&& methods, BuiltInProperty_t&& properties)
{
	auto ptr = runtime->AcquireNewValue<CBuiltInObject>();
	ptr->MakeShared();
	ptr->m_oMethods = std::make_shared<BuiltInMethod_t>(std::move(methods));
	ptr->m_oProperties = std::make_shared<BuiltInProperty_t>(std::move(properties));
	auto internal = ptr->Internal();
	internal->GetAggregateValue().SetAllocator(runtime);
	internal->GetAggregateValue().SetRuntimeInformation(&runtime->GetInformation()->m_oAllMembers);
	return ptr;
}

void CBuiltInObject::Release() {

	assert(m_pAllocator);
	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	m_pAllocator->FreeValue<CBuiltInObject>(this);
	ReleaseShared();
}

IValue* CBuiltInObject::Copy() {

	CBuiltInObject* ptr = m_pAllocator->AcquireNewValue<CBuiltInObject>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();
	ptr->m_oMethods = m_oMethods;
	ptr->m_oProperties = m_oProperties;
	return ptr;
}

IValue* CBuiltInObject::GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) {

	if (m_oMethods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, m_oMethods, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(ctx, this);
	}

	return CObjectValue::GetAggregate(ctx, memberIdx);
}

CBuiltInObjectPairs::~CBuiltInObjectPairs() = default;

void CBuiltInObjects::AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructorFunc)
{
	m_arrData.push_back({ name, { constructorFunc } });
}