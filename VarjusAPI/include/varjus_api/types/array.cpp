#include "array.hpp"

#include "internal/object_declarations.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"
#include "linter/context.hpp"

#include <sstream>

CArrayValue* CArrayValue::Construct(CProgramRuntime* const runtime, IValues&& values)
{
	auto ptr = runtime->AcquireNewValue<CArrayValue>();
	ptr->MakeShared();
	auto internal = ptr->Internal();
	internal->Set(ptr->GetShared(), runtime, std::move(values));
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

CInternalArrayValue* CArrayValue::Internal() {
	return GetShared().get();
}
CInternalArrayValue* CArrayValue::Internal() const {
	return GetShared().get();
}

IValue* CArrayValue::Index([[maybe_unused]] CRuntimeContext* const ctx, IValue* vIndex)
{
	if (!vIndex->IsIntegral())
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("array accessor must be integral, but is \"{}\""), vIndex->TypeAsString()));

	auto index = vIndex->ToUInt();

	auto& vec = GetShared()->GetContent().GetVariables();

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
	auto& vars = GetShared()->GetContent().GetVariables();

	IValues results(vars.size());

	for (std::size_t i = {}; auto& var : vars) {
		results[i] = var->GetValue()->Copy();
		i++;
	}

	return results;
}

CChildVariable* CArrayValue::PushVariable(CProgramRuntime* const runtime, IValue* var) {
	return GetShared()->GetContent().PushVariable(runtime, var);
}
CChildVariable* CArrayValue::PushFrontVariable(CProgramRuntime* const runtime, IValue* var){
	return GetShared()->GetContent().PushFrontVariable(runtime, var);
}

//static bool SelfRef(CProgramRuntime* const runtime, IValue* var, const ArrayOwner& owner)
//{
//	if (auto arr = var->ToArray()) {
//		if (auto arrayPtr = owner.lock()) {
//			return arr->GetSharedPointer() == reinterpret_cast<std::size_t>(arrayPtr.get());
//		}
//		else {
//			throw CRuntimeError(runtime, VSL("internal error within CArrayContent::SelfRef"));
//		}
//	}
//
//	return false;
//}

CChildVariable* CArrayContent::PushVariable(CProgramRuntime* const runtime, IValue* var)
{
	//const auto isRef = SelfRef(runtime, var, m_pArrayOwner);
	auto ret =  m_oVariables.emplace_back(CChildVariable::Construct(runtime, var, m_pArrayOwner));
	//if (isRef)
	//	ret->m_bSelfCapturing = true;

	return ret;
}
CChildVariable* CArrayContent::PushFrontVariable(CProgramRuntime* const runtime, IValue* var)
{
	//SelfRef(runtime, var, m_pArrayOwner);
	return *m_oVariables.insert(m_oVariables.begin(), CChildVariable::Construct(runtime, var, m_pArrayOwner));
}
CInternalArrayValue::~CInternalArrayValue() = default;

void CInternalArrayValue::Release()
{
	for (auto& v : m_oValue.GetVariables()) {
		v->Release();
	}

	m_oValue.m_pArrayOwner.reset();
}
void CInternalArrayValue::Set(const ArrayOwner& self, CProgramRuntime* const runtime, VectorOf<IValue*>&& v){
	
	assert(self.lock());
	m_oValue.GetVariables() = runtime->AcquireNewChildVariables(v.size(), self);
	m_oValue.m_pArrayOwner = self;

	for (auto i = size_t(0); auto& var : m_oValue.GetVariables()) {
		var->SetValue(v[i++]);
	}
}

std::size_t CInternalArrayValue::Length() const noexcept
{
	return m_oValue.GetVariables().size();
}
VarjusString CArrayValue::ValueAsString() const
{
	auto& vec = Internal()->GetContent().GetVariables();

	if (vec.empty())
		return VSL("[]");

	VarjusString ss;

	for (const auto& v : vec) {
		const auto ptr = v->GetValue();
		ss += fmt::format(VSL("{}, "), ptr->GetSharedPointer() == this->GetSharedPointer() ? VSL("...") : ptr->ValueAsString());
	}
	ss.erase(ss.size() - 2, 2);

	return VSL("[ ") + ss + VSL(" ]");
}


