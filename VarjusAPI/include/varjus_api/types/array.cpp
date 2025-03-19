#include "array.hpp"

#include "internal/object_declarations.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"

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
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("array accessor must be integral, but is \"{}\""), vIndex->TypeAsString()));

	auto index = vIndex->ToUInt();

	auto& vec = GetShared()->GetVariables();

	if (index >= vec.size())
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("array index {} out of bounds (len: {})"), index, Internal()->Length()));

	return vec[index]->GetValue();
}
IValue* CArrayValue::GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx)
{

	auto& obj = m_pAllocator->GetDefaultObject<CArrayValue>();
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

IValues CArrayValue::ToIterable() const
{
	auto& vars = GetVariables();
	IValues results(vars.size());

	for (std::size_t i = {}; auto& var : GetVariables()) {
		results[i] = var->GetValue()->Copy();
		i++;
	}

	return results;
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
VarjusString CArrayValue::ValueAsString() const
{
	auto& vec = Internal()->GetVariables();

	if (vec.empty())
		return VSL("[]");

	VarjusString ss;

	for (const auto& v : vec)
		ss += fmt::format(VSL("{}, "), v->GetValue()->ValueAsString());

	ss.erase(ss.size() - 2, 2);

	return VSL("[ ") + ss + VSL(" ]");
}


