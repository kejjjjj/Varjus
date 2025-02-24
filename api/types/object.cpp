#include "object.hpp"

#include "internal/object_declarations.hpp"

#include "api/internal/runtime.hpp"
#include "api/internal/variables.hpp"
#include "api/internal/structure.hpp"

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

	const auto key = index->ValueAsString();

	if (!CFileContext::m_oAllMembers.Contains(key)) {
		throw CRuntimeError(m_pAllocator, std::format("this aggregate doesn't have the attribute \"{}\"", key));
	}

	return Internal()->GetAggregateValue().ElementLookup(CFileContext::m_oAllMembers.At(key));
}
IValue* CObjectValue::GetAggregate(std::size_t memberIdx) {

	if (m_oMethods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(m_pAllocator, this);
	}

	return Internal()->GetAggregateValue().ElementLookup(memberIdx);
}

std::string CObjectValue::ValueAsString() const 
{
	std::stringstream ss;

	for (const auto& [key, value] : GetShared()->GetAggregateValue().Iterator()) {
		ss << "    " << CFileContext::m_oAllMembers.At(key);
		ss << ": " << value->GetValue()->ValueAsString() << ",\n";
	}

	auto result = ss.str();

	if (result.empty())
		return "{}";

	result.erase(result.size() - 2, 2);

	return "{\n" + result + "\n}";
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




