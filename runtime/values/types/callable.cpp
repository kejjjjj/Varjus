#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
#include "runtime/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "callable.hpp"

#include "linter/modules/references.hpp"

IValue* CCallableValue::Copy()
{
	auto ptr = CProgramRuntime::AcquireNewValue<CCallableValue>();
	ptr->MakeShared();
	auto& var = (ptr->GetShared() = GetShared());

	for (auto& [k, va] : var->GetCaptures())
		va->RefCount()++; //increase ref count so that these don't get destroyed

	return ptr;

}
void CCallableValue::Release()
{
	if (SharedRefCount() == 1) {
	}
	Get().Release(); //always decrement ref count

	ReleaseInternal();
	CProgramRuntime::FreeValue<CCallableValue>(this);
	ReleaseShared();
}
IValue* CCallableValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	auto internal = Internal();
	auto callable = internal->GetCallable();

	CRuntimeContext newContext = {
		.m_pModule = internal->m_bRequiresModuleChange
			? CProgramRuntime::GetModuleByIndex(internal->m_uModule)
			: ctx->m_pModule,
		.m_pFunction = ctx->m_pFunction
	};

	auto ret = callable->Execute(&newContext, const_cast<IValues&>(args), internal->GetCaptures());

	assert(ret);

	return ret;
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
			? CProgramRuntime::GetModuleByIndex(var.m_uModuleIndex)
			: ctx->m_pModule;

		if (var.m_bBelongsToDifferentModule) {
			m_oCaptures[var] = activeModule->GetGlobalVariableByIndex(var.m_uIndex)->Copy();
		}
		else {
			m_oCaptures[var] = ctx->m_pFunction->GetVariableByRef(var)->Copy();
		}
	}
}
void CInternalCallableValue::Release()
{
	auto end = m_oCaptures.end();
	for (auto it = m_oCaptures.begin(); it != end; ) {

		assert(it->second->RefCount() != 0);

		if (it->second->Release()) {
			it = m_oCaptures.erase(it);
		} else {
			++it; 
		}
	}
}
