#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
#include "runtime/structure.hpp"
#include "callable.hpp"

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

	auto ret = callable->Execute(ctx, const_cast<IValues&>(args), internal->GetCaptures());

	assert(ret);

	return ret;
}

CInternalCallableValue* CCallableValue::Internal() {
	return GetShared().get();
}
CInternalCallableValue* CCallableValue::Internal() const {
	return GetShared().get();
}
void CInternalCallableValue::SetCaptures(CRuntimeContext* const ctx, const VectorOf<VariableIndex>& captures)
{

	for (auto& varIndex : captures)
		m_oCaptures[varIndex] = ctx->m_pFunction->GetVariableByIndex(varIndex)->Copy();

}
void CInternalCallableValue::Release()
{
	auto end = m_oCaptures.end();
	for (auto it = m_oCaptures.begin(); it != end; ) {

		if (!it->second->GetValue() || it->second->Release()) {
			it = m_oCaptures.erase(it);
		} else {
			++it; 
		}
	}
}
