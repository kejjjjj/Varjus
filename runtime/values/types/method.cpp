//#include "runtime/runtime.hpp"
//#include "runtime/functions/rtfunction.hpp"
//#include "runtime/variables.hpp"
//#include "runtime/structure.hpp"
//#include "runtime/modules/rtmodule.hpp"
//#include "method.hpp"
//
//#include "linter/modules/references.hpp"
//
//IValue* CCallableValue::Copy()
//{
//	auto ptr = CProgramRuntime::AcquireNewValue<CCallableValue>();
//	ptr->MakeShared();
//	auto& var = (ptr->GetShared() = GetShared());
//
//	for (auto& [k, va] : var->GetCaptures())
//		va->RefCount()++; //increase ref count so that these don't get destroyed
//
//	//if (m_pOwner && m_pOwner->m_bSelfCapturing)
//	//	m_pOwner->RefCount()++;
//
//	return ptr;
//
//}
//void CMethodValue::Release()
//{
//	Get().Release(); //always decrement ref count
//
//	ReleaseInternal();
//	CProgramRuntime::FreeValue<CMethodValue>(this);
//	ReleaseShared();
//}
//IValue* CMethodValue::Call(CRuntimeContext* const ctx, IValue* _this, const IValues& args)
//{
//	auto internal = Internal();
//	auto callable = internal->GetCallable();
//
//
//
//	auto ret = callable->Execute(ctx, const_cast<IValues&>(args), internal->GetCaptures());
//
//	assert(ret);
//
//	return ret;
//}
//
//CInternalCallableValue* CCallableValue::Internal() {
//	return GetShared().get();
//}
//CInternalCallableValue* CCallableValue::Internal() const {
//	return GetShared().get();
//}
//void CInternalCallableValue::SetCaptures(CRuntimeContext* const ctx, const VectorOf<CCrossModuleReference>& captures)
//{
//
//	for (auto& var : captures) {
//
//		auto activeModule = var.m_bBelongsToDifferentModule
//			? CProgramRuntime::GetModuleByIndex(var.m_uModuleIndex)
//			: ctx->m_pModule;
//
//
//		CVariable* variable{ nullptr };
//
//		if (var.m_bBelongsToDifferentModule) {
//			variable = activeModule->GetGlobalVariableByIndex(var.m_uIndex);
//		}
//		else {
//			variable = ctx->m_pFunction->GetVariableByRef(var);
//		}
//
//		m_oCaptures[var] = variable->m_bSelfCapturing ? variable : variable->Copy();
//
//	}
//}
//void CInternalCallableValue::Release()
//{
//
//	for (auto it = m_oCaptures.begin(); it != m_oCaptures.end(); ) {
//
//		auto& [ref, variable] = *it;
//
//		if (variable->m_bSelfCapturing && variable->RefCount() == 0 || variable->Release()) {
//
//			if (m_oCaptures.empty()) // why is this needed???
//				break;
//
//			it = m_oCaptures.erase(it);
//		}
//		else {
//			++it;
//		}
//	}
//}
