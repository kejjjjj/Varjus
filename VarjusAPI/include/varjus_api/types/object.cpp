#include "object.hpp"

#include "internal/object_declarations.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"

#include "runtime/exceptions/exception.hpp"
#include "linter/context.hpp"

#include <sstream>


CObjectValue::~CObjectValue() = default;

CObjectValue* CObjectValue::Construct(CProgramRuntime* const runtime, std::size_t moduleIndex, ObjectInitializer&& values)
{
	auto ptr = runtime->AcquireNewValue<CObjectValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->GetAggregateValue().SetAllocator(runtime);
	internal->GetAggregateValue().SetRuntimeInformation(&runtime->GetInformation()->m_oAllMembers);
	internal->Set(moduleIndex, std::move(values));
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
IValue* CObjectValue::Index(IValue* index) {

	auto members = Internal()->GetAllRuntimeMembers();
	assert(members);

	const auto key = index->ValueAsString();

	if (!members->Contains(key)) {
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("this aggregate doesn't have the attribute \"{}\""), key));
	}

	return Internal()->GetAggregateValue().ElementLookup(members->At(key));
}
IValue* CObjectValue::GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) {

	auto& obj = m_pAllocator->GetDefaultObject<CObjectValue>();
	auto methods = obj.GetMethods();
	assert(methods);
	if (methods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, methods, this->Copy());
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

	auto members = Internal()->GetAllRuntimeMembers();

	for (const auto& [key, value] : GetShared()->GetAggregateValue().Iterator()) {
		assert(members);
		ss += fmt::format(VSL("    {}: {},\n"), members->At(key), value->GetValue()->ValueAsString());
	}

	if (ss.empty())
		return VSL("{}");

	ss.erase(ss.size() - 2, 2);
	return VSL("{\n") + ss + VSL("\n}");
}
/***********************************************************************
 > 
***********************************************************************/
CInternalObjectValue::~CInternalObjectValue() = default;

void CInternalObjectValue::Release()
{
	GetAggregateValue().Release();
}
void CInternalObjectValue::Set(std::size_t moduleIndex, ObjectInitializer&& v) {

	m_oValue.SetModuleIndex(moduleIndex);
	for (auto& [key, value] : v) {
		m_oValue.AddAttribute(key, value);
	}
}




