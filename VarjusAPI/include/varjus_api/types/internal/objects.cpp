#include "linter/context.hpp"
#include "object_declarations.hpp"
#include "objects.hpp"
#include "runtime/misc/defs.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"

#include <iostream>

using namespace Varjus;

CBuiltInObject::CBuiltInObject() : CObjectValue() {}
CBuiltInObject::~CBuiltInObject() = default;

CBuiltInObject* CBuiltInObject::Construct(Varjus::CProgramRuntime* const runtime, BuiltInMethod_t&& methods, BuiltInProperty_t&& properties)
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
IValue* CBuiltInObject::Index(Varjus::CRuntimeContext* const ctx, IValue* index) {
	const auto key = index->ValueAsEscapedString();

	if (!m_pAllocator->ContainsKey(key)) {
		throw CRuntimeError(m_pAllocator, Varjus::fmt::format(VSL("this aggregate doesn't have the attribute \"{}\""), key));
	}

	const auto memberIdx = m_pAllocator->StringToKey(key);

	if (m_oMethods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		VARJUS_METHOD_BIND(v, m_oMethods, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(ctx, this);
	}

	return Internal()->GetAggregateValue().ElementLookup(memberIdx);
}
IValue* CBuiltInObject::GetAggregate(Varjus::CRuntimeContext* const ctx, std::size_t memberIdx) {

	if (m_oMethods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		VARJUS_METHOD_BIND(v, m_oMethods, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(ctx, this);
	}

	return CObjectValue::GetAggregate(ctx, memberIdx);
}

VarjusString CBuiltInObject::ValueAsString() const
{
	assert(m_oMethods && m_oMethods->m_pInfo);
	assert(m_oProperties && m_oProperties->m_pInfo);

	VarjusString ss;
	for (auto& [id, _] : *m_oMethods) {
		ss += Varjus::fmt::format(VSL("\"{}\":\"method\","), m_oMethods->m_pInfo->m_oAllMembers.At(id));
	}
	for (auto& [id, _] : *m_oProperties) {
		ss += Varjus::fmt::format(VSL("\"{}\":\"property\","), m_oProperties->m_pInfo->m_oAllMembers.At(id));
	}
	if (ss.empty())
		return VSL("{}");

	ss.pop_back(); // remove comma

	return VSL("{") + ss + VSL("}");

}

CBuiltInObjectPairs::~CBuiltInObjectPairs() = default;

void CBuiltInObjects::AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructorFunc)
{
	m_arrData.push_back({ name, { constructorFunc } });
}