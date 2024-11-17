#include "object.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"

#include <sstream>


CObjectValue::~CObjectValue() = default;

void CObjectValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}
	
	ReleaseInternal();
	CProgramRuntime::FreeValue<CObjectValue>(this);
	ReleaseShared();
}

CObjectValue* CObjectValue::Copy() {
	CObjectValue* ptr = CProgramRuntime::AcquireNewValue<CObjectValue>();
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
IValue* CObjectValue::GetAggregate(std::size_t memberIdx) {
	return Internal()->GetAggregateValue().ElementLookup(memberIdx);
}

std::string CObjectValue::ValueAsString() const 
{
	std::stringstream ss;

	for (const auto& [key, value] : GetShared()->GetAggregateValue().Iterator()) {
		ss << CProgramRuntime::GetContext()->m_oAllMembers.At(key);
		ss << ": " << value->GetValue()->ValueAsString() << ", ";
	}

	auto result = ss.str();

	if (result.empty())
		return "{}";

	result.erase(result.size() - 2, 2);

	return "{ " + result + " }";
}
/***********************************************************************
 > 
***********************************************************************/
CInternalObjectValue::~CInternalObjectValue() = default;

void CInternalObjectValue::Release()
{
	GetAggregateValue().Release();
}
void CInternalObjectValue::Set(ObjectInitializer&& v) {

	for (auto& [key, value] : v) {
		m_oValue.AddAttribute(key, value);
	}
}




