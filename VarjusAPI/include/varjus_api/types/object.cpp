#include "object.hpp"

#include "internal/object_declarations.hpp"

#include "runtime/misc/defs.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "linter/context.hpp"

#include <sstream>

using namespace Varjus;

CObjectValue::~CObjectValue() = default;

void CObjectValue::AddAttribute(IValue* const key, IValue* value)
{
	auto& aggregate = Internal()->GetAggregateValue();

	auto members = Internal()->GetAllRuntimeMembers();
	assert(members);

	//insert the member if necessary
	auto var = aggregate.AddAttribute((*members)[key->ValueAsEscapedString()]);
	var->SetValue(value->Copy());
}
CObjectValue* CObjectValue::Construct(Varjus::CProgramRuntime* const runtime, __ObjectValues&& values)
{
	auto ptr = runtime->AcquireNewValue<CObjectValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->GetAggregateValue().SetAllocator(runtime);
	internal->GetAggregateValue().SetRuntimeInformation(&runtime->GetInformation()->m_oAllMembers);

	for (auto&& [k, v] : values) {
		ptr->AddAttribute(k, v);

		k->Release();
		v->Release();
	}

	return ptr;
}

CObjectValue* CObjectValue::_ConstructInternal(Varjus::CProgramRuntime* const runtime, __ObjectInitializer&& values)
{
	auto ptr = runtime->AcquireNewValue<CObjectValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->GetAggregateValue().SetAllocator(runtime);
	internal->GetAggregateValue().SetRuntimeInformation(&runtime->GetInformation()->m_oAllMembers);
	internal->Set(std::move(values));
	return ptr;
}

void CObjectValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}
	
	ReleaseInternal();
	m_pAllocator->FreeValue<CObjectValue>(this);
	ReleaseShared();
}

IValue* CObjectValue::Copy() {
	CObjectValue* ptr = m_pAllocator->AcquireNewValue<CObjectValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();
	return ptr;
}

CInternalObjectValue* CObjectValue::Internal() {
	return GetShared().get();
}
CInternalObjectValue* CObjectValue::Internal() const {
	return GetShared().get();
}
IValue* CObjectValue::Index([[maybe_unused]] Varjus::CRuntimeContext* const ctx, IValue* index) {
	const auto key = index->ValueAsEscapedString();

	if (!m_pAllocator->ContainsKey(key)) {
		throw CRuntimeError(m_pAllocator, Varjus::fmt::format(VSL("this aggregate doesn't have the attribute \"{}\""), key));
	}

	return Internal()->GetAggregateValue().ElementLookup(m_pAllocator->StringToKey(key));
}
IValue* CObjectValue::GetAggregate(Varjus::CRuntimeContext* const ctx, std::size_t memberIdx) {

	auto& obj = m_pAllocator->GetDefaultObject<CObjectValue>();
	auto methods = obj.GetMethods();
	assert(methods);

	if (methods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		VARJUS_METHOD_BIND(v, methods, this->Copy());
		return v;
	}

	auto properties = obj.GetProperties();
	assert(properties);
	if (properties->contains(memberIdx)) {
		return properties->at(memberIdx)(ctx, this);
	}

	return Internal()->GetAggregateValue().ElementLookup(memberIdx);
}

VarjusString CObjectValue::ValueAsString() const 
{
	VarjusString ss;

	for (const auto& [key, value] : GetShared()->GetAggregateValue().Iterator()) {
		ss += Varjus::fmt::format(VSL("\"{}\":{},"), m_pAllocator->KeyToString(key), value->GetValue()->ValueAsString());
	}

	if (ss.empty())
		return VSL("{}");
	ss.pop_back(); // remove comma

	return VSL("{") + ss + VSL("}");
}

/***********************************************************************
 > 
***********************************************************************/
CInternalObjectValue::~CInternalObjectValue() = default;

void CInternalObjectValue::Release()
{
	GetAggregateValue().Release();
}
void CInternalObjectValue::Set(__ObjectInitializer&& v) {

	for (auto& [key, value] : v) {
		m_oValue.AddAttribute(key, value);
	}
}




