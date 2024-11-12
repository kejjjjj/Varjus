#include "array.hpp"
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

	const auto context = CProgramRuntime::GetContext();
	elems.push_back(context->m_oAllMembers["length"]);

	once = false;
	return elems;
}


CArrayValue::CArrayValue(VectorOf<IValue*>&& v) :
	CValue(std::make_shared<CInternalArrayValue>(std::move(v))){
}
CArrayValue::~CArrayValue() = default;


void CArrayValue::CreateOwnership()
{
	m_oValue = std::make_shared<CInternalArrayValue>();
}

void CArrayValue::Release(){

	if(m_oValue.use_count() == 1)
		m_oValue->Release();

	ReleaseInternal();
	CProgramRuntime::FreeValue<CArrayValue>(this);
	m_oValue.reset();
}
CArrayValue* CArrayValue::MakeShared() const
{
	CArrayValue* ptr = CProgramRuntime::AcquireNewValue<CArrayValue>();
	ptr->GetRawValue() = m_oValue;
	return ptr;
}
IValue* CArrayValue::Copy(){

	return MakeShared();
}

CArrayValue* CArrayValue::ToArray(){
	return this;
}
CInternalArrayValue* CArrayValue::Internal() {
	return m_oValue.get();
}
CInternalArrayValue* CArrayValue::Internal() const {
	return m_oValue.get();
}

IValue* CArrayValue::Index(std::int64_t index)
{
	auto& vec = m_oValue->GetVariables();

	if (index < 0 || static_cast<size_t>(index) >= vec.size())
		throw CRuntimeError("array index out of bounds");

	return vec[static_cast<size_t>(index)]->GetValue();
}
IValue* CArrayValue::GetAggregate(std::size_t memberIdx)
{
	auto value = Internal()->GetAggregateValue().ElementLookup(memberIdx);

	if (memberIdx == ARRAY_LENGTH) {
		assert(value->IsIntegral());
		value->AsInt() = static_cast<std::int64_t>(Internal()->Length());
	}

	return value;
}
CInternalArrayValue::CInternalArrayValue(VectorOf<IValue*>&& v){
	Set(std::move(v));
}
CInternalArrayValue::~CInternalArrayValue() = default;

void CInternalArrayValue::Release()
{
	GetAggregateValue().Release();

	for (auto& v : GetVariables()) {
		auto& value = v.get()->GetValue();
		value->Release();
		value = nullptr;
		CProgramRuntime::m_oVariablePool.Release(std::move(v));
	}

	//ReleaseInternal();

}
void CInternalArrayValue::Set(VectorOf<IValue*>&& v){
	m_oValue.m_oVariables = CProgramRuntime::m_oVariablePool.Acquire(v.size());

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
		ss << v.get()->GetValue()->ValueAsString() << ", ";

	auto result = ss.str();
	result.erase(result.size() - 2, 2);

	return '[' + result + ']';
}
