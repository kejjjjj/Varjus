#include "method.hpp"
#include "runtime/runtime.hpp"

IValue* __method__internal::CreateUndefinedVar()
{
	return CProgramRuntime::AcquireNewValue<IValue>();
}
