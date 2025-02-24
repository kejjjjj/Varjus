#include "string.hpp"

#include "internal/object_declarations.hpp"

#include "api/internal/runtime.hpp"
#include "api/internal/structure.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"


CStringValue* CStringValue::Construct(CProgramRuntime* const runtime, const std::string& v)
{

	auto str = runtime->AcquireNewValue<CStringValue>();
	auto internal = str->Internal();
	str->MakeUnique();
	internal->Set(v);
	//internal->GetAggregateValue().AddAttribute(LENGTH_PROPERTY);
	return str;
}

IValue* CStringValue::Copy(){
	auto v = Construct(m_pAllocator, Internal()->GetString());
	return v;
}
void CStringValue::Release()
{
	Internal()->Release();

	ReleaseInternal();
	m_pAllocator->FreeValue<CStringValue>(this);
}
CInternalStringValue* CStringValue::Internal() {
	return &Get();
}
const CInternalStringValue* CStringValue::Internal() const {
	return &Get();
}
IValue* CStringValue::Index(IValue* vIndex)
{
	if (!vIndex->IsIntegral())
		throw CRuntimeError(m_pAllocator, std::format("array accessor must be integral, but is \"{}\"", vIndex->TypeAsString()));

	auto index = vIndex->ToInt();

	if (index < 0 || static_cast<size_t>(index) >= Internal()->Length())
		throw CRuntimeError(m_pAllocator, std::format("string index {} out of bounds (len: {})", index, Internal()->Length()));

	auto newStr = std::string(size_t(1), Internal()->GetString()[static_cast<size_t>(index)]);
	auto v = Construct(m_pAllocator, newStr);
	v->MakeImmutable(); //cannot modify parts
	return v;
}
IValue* CStringValue::GetAggregate(std::size_t memberIdx)
{
	if (m_oMethods->contains(memberIdx)) {
		auto v = m_pAllocator->AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(m_pAllocator, this);
	}

	assert(false);
	return nullptr;

}
CInternalStringValue::~CInternalStringValue() = default;

void CInternalStringValue::Release()
{
}
void CInternalStringValue::Set(const std::string& value) {
	m_oValue.m_sString = value;
}

std::size_t CInternalStringValue::Length() const noexcept
{
	return m_oValue.m_sString.length();
}


