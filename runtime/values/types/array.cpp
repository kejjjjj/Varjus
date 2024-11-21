#include "array.hpp"
#include "array_internal/array_builtin.hpp"
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

	auto& m = CProgramRuntime::GetContext()->m_oAllMembers;

	elems.push_back(m["length"]);

	for (auto& [id, method] : CStaticArrayBuiltInMethods::GetIterator()) {
		elems.push_back(id);

	}

	once = false;
	return elems;
}

CArrayValue::~CArrayValue() = default;

void CArrayValue::Release(){

	std::cout << "CArrayValue: " << SharedRefCount() << '\n';
	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CArrayValue>(this);
	ReleaseShared();

}

IValue* CArrayValue::Copy(){

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

	auto value = Internal()->GetAggregateValue().ElementLookup(memberIdx);

	if (value->IsBuiltInMemberCallable()) {

		auto ptr = value->ToBuiltInMemberCallable();
		ptr->SetOwner(GetOwner());
		ptr->GetShared()->m_pThis = this->Copy();

		return ptr;
	}

	if (memberIdx == ARRAY_LENGTH) {
		assert(value->IsIntegral());
		value->AsInt() = static_cast<std::int64_t>(Internal()->Length());
	}


	return value;
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
