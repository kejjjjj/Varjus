#include "string.hpp"

#include "internal/object_declarations.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"

#include "linter/context.hpp"


CStringValue* CStringValue::Construct(CProgramRuntime* const runtime, const VarjusString& v)
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

IValue* CStringValue::Index(IValue* vIndex)
{
	if (!vIndex->IsIntegral())
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("array accessor must be integral, but is \"{}\""), vIndex->TypeAsString()));

	auto index = vIndex->ToUInt();

	if (index >= Internal()->Length())
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("string index {} out of bounds (len: {})"), index, Internal()->Length()));

	auto newStr = VarjusString(size_t(1), Internal()->GetString()[index]);
	auto v = Construct(m_pAllocator, newStr);
	v->MakeImmutable(); //cannot modify parts
	return v;
}
IValue* CStringValue::GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx)
{
	auto& obj = m_pAllocator->GetDefaultObject<CStringValue>();
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

	if (auto info = m_pAllocator->GetInformation()) {
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("this aggregate doesn't have the attribute \"{}\""), info->m_oAllMembers.At(memberIdx)));
	}

	assert(false);
	return nullptr;

}
IValues CStringValue::ToIterable() const
{
	auto& str = Internal()->GetString();

	IValues results(str.size());

	for (std::size_t i = {}; auto var : str) {
		results[i++] = CStringValue::Construct(m_pAllocator, VarjusString(size_t(1), var));
	}

	return results;
}

CInternalStringValue::~CInternalStringValue() = default;

void CInternalStringValue::Release()
{
}
void CInternalStringValue::Set(const VarjusString& value) {
	m_oValue.m_sString = value;
}

