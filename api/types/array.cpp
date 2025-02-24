#include "array.hpp"

#include "internal/object_declarations.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/variables.hpp"
#include "api/internal/structure.hpp"

#include "runtime/exceptions/exception.hpp"
#include "linter/context.hpp"

#include <sstream>

CArrayValue* CArrayValue::Construct(CProgramRuntime* const runtime, IValues&& values)
{
	auto ptr = runtime->AcquireNewValue<CArrayValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->Set(runtime, std::move(values));
	return ptr;
}

CArrayValue::~CArrayValue() = default;

void CArrayValue::Release(){

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	m_pAllocator->FreeValue<CArrayValue>(this);
	ReleaseShared();

}

IValue* CArrayValue::Copy(){
	CArrayValue* ptr = m_pAllocator->AcquireNewValue<CArrayValue>();
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
		throw CRuntimeError(m_pAllocator, std::format("array accessor must be integral, but is \"{}\"", vIndex->TypeAsString()));

	auto index = vIndex->ToInt();

	auto& vec = GetShared()->GetVariables();

	if (index < 0 || static_cast<size_t>(index) >= vec.size())
		throw CRuntimeError(m_pAllocator, "array index out of bounds");

	return vec[static_cast<size_t>(index)]->GetValue();
}
IValue* CArrayValue::GetAggregate(std::size_t memberIdx)
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
void CInternalArrayValue::Set(CProgramRuntime* const runtime, VectorOf<IValue*>&& v){
	m_oValue.m_oVariables = runtime->AcquireNewVariables(v.size());

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


