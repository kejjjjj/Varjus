#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
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
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CCallableValue>(this);
	ReleaseShared();
}
CInternalCallableValue* CCallableValue::Internal() {
	return GetShared().get();
}
CInternalCallableValue* CCallableValue::Internal() const {
	return GetShared().get();
}

void CInternalCallableValue::SetCaptures(CFunction* const thisFunction, const VectorOf<VariableIndex>& captures)
{

	for (auto& varIndex : captures)
		m_oCaptures[varIndex] = thisFunction->GetVariableByIndex(varIndex)->Copy();

}
void CInternalCallableValue::Release()
{
	for (auto it = m_oCaptures.begin(); it != m_oCaptures.end(); ) {

		if (it->second->RefCount() == 1 || !it->second->GetValue() || it->second->Release()) {
			it = m_oCaptures.erase(it);
		} else {
			++it; 
		}
	}
}
