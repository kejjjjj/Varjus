#include "array.hpp"

#include "internal/object_declarations.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/variables.hpp"
#include "api/internal/structure.hpp"

#include "runtime/exceptions/exception.hpp"
#include "linter/context.hpp"

#include <sstream>



VectorOf<ElementIndex>& runtime::__internal::GetAggregateArrayData()
{
	static bool once = true;
	static VectorOf<ElementIndex> elems;

	if (!once)
		return elems;

	//elems.push_back(LENGTH_PROPERTY);
	once = false;
	return elems;
}
CArrayValue* CArrayValue::Construct(IValues&& values)
{
	auto ptr = CProgramRuntime::AcquireNewValue<CArrayValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->Set(std::move(values));
	return ptr;
}

CArrayValue::~CArrayValue() = default;

void CArrayValue::Release(){

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

IValue* CArrayValue::Index(IValue* vIndex)
{
	if (!vIndex->IsIntegral())
		throw CRuntimeError(std::format("array accessor must be integral, but is \"{}\"", vIndex->TypeAsString()));

	auto index = vIndex->ToInt();

	auto& vec = GetShared()->GetVariables();

	if (index < 0 || static_cast<size_t>(index) >= vec.size())
		throw CRuntimeError("array index out of bounds");

	return vec[static_cast<size_t>(index)]->GetValue();
}
IValue* CArrayValue::GetAggregate(std::size_t memberIdx)
{

	if (m_oMethods->contains(memberIdx)) {
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, this->Copy());
		return v;
	}

	if (m_oProperties->contains(memberIdx)) {
		return m_oProperties->at(memberIdx)(this);
	}

	assert(false);
	return nullptr;
}

VectorOf<CVariable*>& CArrayValue::GetVariables() {
	return GetShared()->GetVariables();
}
VectorOf<CVariable*>& CArrayValue::GetVariables() const {
	return GetShared()->GetVariables();
}
CInternalArrayValue::~CInternalArrayValue() = default;

void CInternalArrayValue::Release()
{
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


