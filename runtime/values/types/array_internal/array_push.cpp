#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* CArrayValue::Push(const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& vars = GetShared()->GetVariables();
	auto& newVar = vars.emplace_back(CProgramRuntime::AcquireNewVariable());
	newVar->SetValue(newValues.front());
	return newVar->GetValue();
}
