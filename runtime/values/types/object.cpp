#include "object.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"

#include <sstream>

CObjectValue::CObjectValue(ObjectInitializer&& v)
	: CValue(std::make_shared<CInternalObjectValue>(std::move(v))) {}
CObjectValue::~CObjectValue() = default;

void CObjectValue::CreateOwnership() {
	m_oValue = std::make_shared<CInternalObjectValue>();
}
void CObjectValue::Release() {

	if (m_oValue.use_count() == 1)
		m_oValue->Release();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CObjectValue>(this);
	m_oValue.reset();
}

CObjectValue* CObjectValue::MakeShared() const {
	CObjectValue* ptr = CProgramRuntime::AcquireNewValue<CObjectValue>();
	ptr->Get() = m_oValue;
	return ptr;
}
CObjectValue* CObjectValue::Copy() {
	return MakeShared();
}
CInternalObjectValue* CObjectValue::Internal() {
	return m_oValue.get();
}
CInternalObjectValue* CObjectValue::Internal() const {
	return m_oValue.get();
}
IValue* CObjectValue::GetAggregate(std::size_t memberIdx) {
	return Internal()->GetAggregateValue().ElementLookup(memberIdx);
}

std::string CObjectValue::ValueAsString() const 
{
	std::stringstream ss;

	for (const auto& [key, value] : m_oValue->GetAggregateValue().Iterator()) {
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
CInternalObjectValue::CInternalObjectValue(ObjectInitializer&& v) {
	Set(std::move(v));
}
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




