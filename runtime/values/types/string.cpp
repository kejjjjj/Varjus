#include "string.hpp"

#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"
#include "linter/context.hpp"

CStringValue* CStringValue::Construct(const std::string& v)
{
	auto str = CProgramRuntime::AcquireNewValue<CStringValue>();
	auto internal = str->Internal();
	str->MakeUnique();
	internal->Set(v);
	internal->GetAggregateValue().AddAttribute(ARRAY_LENGTH);
	return str;
}

IValue* CStringValue::Copy(){
	return Construct(Internal()->GetString());
}
void CStringValue::Release()
{
	Internal()->Release();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CStringValue>(this);
}
CInternalStringValue* CStringValue::Internal() {
	return &Get();
}
const CInternalStringValue* CStringValue::Internal() const {
	return &Get();
}
IValue* CStringValue::Index(std::int64_t index)
{
	if (index < 0 || static_cast<size_t>(index) >= Internal()->Length())
		throw CRuntimeError("string index out of bounds");


	auto newStr = std::string(size_t(1), Internal()->GetString()[static_cast<size_t>(index)]);
	auto v = Construct(newStr);
	v->MakeImmutable(); //cannot modify parts
	return v;
}
IValue* CStringValue::GetAggregate(std::size_t memberIdx)
{

	if (auto func = CBuiltInMethods<CStringValue>::LookupMethod(memberIdx)) {
		auto ptr = HasOwner() ? this : this->Copy()->ToCString();
		ptr->m_pMethod = func;
		return ptr;
	}

	auto value = Internal()->GetAggregateValue().ElementLookup(memberIdx);

	if (memberIdx == ARRAY_LENGTH) {
		assert(value->IsIntegral());
		value->AsInt() = static_cast<std::int64_t>(Internal()->Length());
	}

	return value;

}
IValue* CStringValue::Call(CFunction* const thisFunction, const IValues& args)
{
	assert(IsCallable());
	auto ret = CBuiltInMethods<CStringValue>::CallMethod(thisFunction, this, args, m_pMethod);
	m_pMethod = nullptr;
	return ret;
}
CInternalStringValue::~CInternalStringValue() = default;

void CInternalStringValue::Release()
{
	GetAggregateValue().Release();
}
void CInternalStringValue::Set(const std::string& value) {
	m_oValue.m_sString = value;
}

std::size_t CInternalStringValue::Length() const noexcept
{
	return m_oValue.m_sString.length();
}

CStringValue::StringMethods CStringValue::ConstructMethods()
{
	return {
		{"to_upper", {0u, &CStringValue::ToUpper}},
		{"to_lower", {0u, &CStringValue::ToLower}},

	};
}
