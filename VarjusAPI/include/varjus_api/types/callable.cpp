#include "varjus_api/internal/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "callable.hpp"

#include "varjus_api/types/internal/references.hpp"


CCallableValue* CCallableValue::Construct(Varjus::CProgramRuntime* const runtime, CRuntimeFunctionBase* v)
{
	auto ptr = runtime->AcquireNewValue<CCallableValue>();
	ptr->MakeShared();
	ptr->Internal()->SetCallable(v);
	return ptr;
}

IValue* CCallableValue::Copy()
{
	auto ptr = m_pAllocator->AcquireNewValue<CCallableValue>();
	ptr->MakeShared();
	auto& var = (ptr->GetShared() = GetShared());

	for (auto& [k, va] : var->m_oCaptures)
		va->RefCount()++; //increase ref count so that these don't get destroyed

	return ptr;

}
void CCallableValue::Release()
{
	if (SharedRefCount() == 1) {
		if (auto binding = Internal()->GetBinding()) {
			binding->Release();
			Internal()->Bind(nullptr);
		}

	}

	Get().Release(this); //always decrement ref count
	

	ReleaseInternal();
	m_pAllocator->FreeValue<CCallableValue>(this);
	ReleaseShared();
	

}
IValue* CCallableValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	if (ctx->m_pRuntime->ExceptionThrown())
		return ctx->m_pRuntime->GetExceptionValue();

	auto internal = Internal();
	auto callable = internal->GetCallable();

	CRuntimeContext newContext = {
		.m_pRuntime = ctx->m_pRuntime,
		.m_pModule = internal->m_bRequiresModuleChange
			? ctx->m_pRuntime->GetModuleByIndex(internal->m_uModule)
			: ctx->m_pModule,
		.m_pFunction = ctx->m_pFunction
	};

	auto ret = callable->ExecuteFunction(&newContext, internal->GetBinding(), const_cast<IValues&>(args), internal->GetCaptures());
	assert(ret);
	return ret;
}
VarjusString CCallableValue::ValueAsString() const {
	return Varjus::fmt::format(VSL("\"function->({})\""), GetShared()->GetCallable()->GetName());
}
VarjusString CCallableValue::ValueAsEscapedString() const {
	return Varjus::fmt::format(VSL("function->({})"), GetShared()->GetCallable()->GetName());
}
CInternalCallableValue* CCallableValue::Internal() {
	return GetShared().get();
}
CInternalCallableValue* CCallableValue::Internal() const {
	return GetShared().get();
}
void CInternalCallableValue::SetCaptures(CRuntimeContext* const ctx, const VectorOf<CCrossModuleReference>& captures)
{

	for (auto& var : captures) {

		auto activeModule = var.m_bBelongsToDifferentModule
			? ctx->m_pRuntime->GetModuleByIndex(var.m_uModuleIndex)
			: ctx->m_pModule;


		CVariable* variable{ nullptr };

		if (var.m_bBelongsToDifferentModule) {
			variable = activeModule->GetGlobalVariableByIndex(var.m_uIndex);
		}
		else {
			variable = ctx->m_pFunction->GetVariableByRef(var);
		}

		//has to be a copy
		//for example an anonymous lambda captures this and it never gets copied, ref count doesn't increase
		//original variable gets deleted (ref count is 0)
		//the capture gets deleted (ref count is already 0 -> BUG)
		m_oCaptures[var] = variable->Copy();
		
	}
}
void CInternalCallableValue::Release(CCallableValue* const _this)
{
	const auto thisIsProblematic = _this->HasOwner() && _this->GetOwner()->m_bSelfCapturing && _this->GetOwner()->RefCount() == 0;

	for (auto it = m_oCaptures.begin(); it != m_oCaptures.end();) {

		auto& [ref, variable] = *it;
		
		if (thisIsProblematic && variable->GetValue() == _this) {
			it++;
			continue;
		}

		assert(variable->RefCount());
		if (!variable->GetValue() || variable->Release()) {
			it = m_oCaptures.erase(it);
			continue;
		}
		it++;
	}
}
