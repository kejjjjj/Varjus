#include "varjus_api/internal/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "callable.hpp"

#include "varjus_api/types/internal/references.hpp"


CCallableValue* CCallableValue::Construct(CProgramRuntime* const runtime, CRuntimeFunctionBase* v)
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

	for (auto& [k, va] : var->GetCaptures())
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

	Get().Release(); //always decrement ref count

	ReleaseInternal();
	m_pAllocator->FreeValue<CCallableValue>(this);
	ReleaseShared();
}
IValue* CCallableValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
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

		m_oCaptures[var] = variable->m_bSelfCapturing ? variable : variable->Copy();
		
	}
}
//void CInternalCallableValue::SetCallable(CRuntimeFunctionBase* c) noexcept
//{
//	m_pCallable = c;
//}
void CInternalCallableValue::Release()
{

	for (auto it = m_oCaptures.begin(); it != m_oCaptures.end(); ) {

		auto& [ref, variable] = *it;

		if ((variable->m_bSelfCapturing && variable->RefCount() == 0) || variable->Release()) {

			if (m_oCaptures.empty()) // why is this needed???
				break;

			it = m_oCaptures.erase(it);
		} else {
			++it; 
		}
	}
}
