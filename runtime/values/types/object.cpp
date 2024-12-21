#include "object.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"

#include <sstream>


CObjectValue::~CObjectValue() = default;

CObjectValue* CObjectValue::Construct(std::size_t moduleIndex, ObjectInitializer&& values)
{
	auto ptr = CProgramRuntime::AcquireNewValue<CObjectValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->Set(moduleIndex, std::move(values));
	return ptr;
}

void CObjectValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}
	
	ReleaseInternal();
	CProgramRuntime::FreeValue<CObjectValue>(this);
	ReleaseShared();
}

IValue* CObjectValue::Copy() {
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
		ss << "    " << /*CProgramRuntime::GetContext()->m_oAllMembers.At(key)*/ "implement me";
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




