#include "array.hpp"
#include "internal/builtin_methods.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"

#include <sstream>

VectorOf<ElementIndex>& runtime::__internal::GetAggregateArrayData()
{
	static bool once = true;
	static VectorOf<ElementIndex> elems;

	if (!once)
		return elems;

	elems.push_back(LENGTH_PROPERTY);
	once = false;
	return elems;
}
CArrayValue* CArrayValue::Construct(IValues&& values)
{
	auto ptr = CProgramRuntime::AcquireNewValue<CArrayValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->Set(std::move(values));
	internal->GetAggregateValue().Setup(0, runtime::__internal::GetAggregateArrayData());
	return ptr;
}

CArrayValue::~CArrayValue() = default;

void CArrayValue::Release(){

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CDataTypeMethods::Release();
	CProgramRuntime::FreeValue<CArrayValue>(this);
	ReleaseShared();

}

IValue* CArrayValue::Copy(){

	if (auto c = CDataTypeMethods::Copy())
		return c;

	CArrayValue* ptr = CProgramRuntime::AcquireNewValue<CArrayValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();
	return ptr;
}

CArrayValue* CArrayValue::ToArray(){
	return this;
}
CInternalArrayValue* CArrayValue::Internal() {
	return GetShared().get();
}
CInternalArrayValue* CArrayValue::Internal() const {
	return GetShared().get();
}

IValue* CArrayValue::Index(std::int64_t index)
{
	auto& vec = GetShared()->GetVariables();

	if (index < 0 || static_cast<size_t>(index) >= vec.size())
		throw CRuntimeError("array index out of bounds");

	return vec[static_cast<size_t>(index)]->GetValue();
}
IValue* CArrayValue::GetAggregate(std::size_t memberIdx)
{

	if (auto func = CDataTypeMethods::FindMethod(memberIdx)) {
		auto newValue = HasOwner() ? this : this->Copy()->ToArray();
		newValue->SetMethod(func);
		return newValue;
	}

	auto property = Internal()->GetAggregateValue().ElementLookup(memberIdx);

	if (memberIdx == LENGTH_PROPERTY) {
		assert(property->IsIntegral());
		property->AsInt() = static_cast<std::int64_t>(Internal()->Length());
	}

	return property;
}

IValue* CArrayValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	assert(IsCallable());
	auto ret = CBuiltInMethods<CArrayValue>::CallMethod(ctx, this, args, GetMethod());
	CDataTypeMethods::Release();
	return ret;
}

CInternalArrayValue::~CInternalArrayValue() = default;

void CInternalArrayValue::Release()
{
	GetAggregateValue().Release();

	for (auto& v : GetVariables()) {
		v->Release();
	}
}
void CInternalArrayValue::Set(VectorOf<IValue*>&& v){
	m_oValue.m_oVariables = CProgramRuntime::AcquireNewVariables(v.size());

	for (auto i = size_t(0); auto & var : m_oValue.m_oVariables)
		var->SetValue(v[i++]);
}

std::size_t CInternalArrayValue::Length() const noexcept
{
	return GetVariables().size();
}
std::string CArrayValue::ValueAsString() const
{
	auto& vec = Internal()->GetVariables();

	if (vec.empty())
		return "[]";

	std::stringstream ss;

	for (const auto& v : vec)
		ss << v->GetValue()->ValueAsString() << ", ";

	auto result = ss.str();
	result.erase(result.size() - 2, 2);

	return "[ " + result + " ]";
}


