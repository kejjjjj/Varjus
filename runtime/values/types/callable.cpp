#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
#include "callable.hpp"

IValue* CCallableValue::Copy()
{
	if (IsShared()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CCallableValue>();
		ptr->MakeShared();
		ptr->GetShared() = GetShared();
		return ptr;
	}

	auto v = CProgramRuntime::AcquireNewValue<CCallableValue>(Get());
	v->GetCaptures() = m_oCaptures;

	for (auto& [k, va] : v->GetCaptures())
		va->RefCount()++;

	return v;
}
void CCallableValue::Release()
{
	if (IsShared())
		ReleaseShared();

	ReleaseInternal();

	for (auto& [id, var] : m_oCaptures)
		var->Release();

	CProgramRuntime::FreeValue<CCallableValue>(this);
}

void CCallableValue::SetCaptures(CFunction* const thisFunction, const VectorOf<VariableIndex>& captures)
{

	for (auto& varIndex : captures)
		m_oCaptures[varIndex] = thisFunction->GetVariableByIndex(varIndex)->Copy();

}
