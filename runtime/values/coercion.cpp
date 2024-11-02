#include "coercion.hpp"
#include "runtime/runtime.hpp"
#include "types/types.hpp"

#include "runtime/exceptions/exception.hpp"

#include <cassert>
#include <format>

std::tuple<IValue*, IValue*, IValue*> Coerce(IValue* lhs, IValue* rhs)
{
	if (lhs->Type() == rhs->Type())
		return { lhs, rhs, nullptr };

	if (lhs->Type() < rhs->Type()) {
		auto data = CoerceInternal(lhs, rhs, true);
		return { data.GetLHS(), data.GetRHS(), data.allocated };
	} 

	auto data = CoerceInternal(rhs, lhs, false);
	return { data.GetLHS(), data.GetRHS(), data.allocated };
}
CCoercionOperands CoerceInternal(IValue* weaker, IValue* stronger, bool lhsIsWeak)
{
	assert(weaker->Type() != stronger->Type());

	if (!weaker->Coerceable())
		throw CRuntimeError(std::format("cannot coerce from \"{}\" to \"{}\"", weaker->TypeAsString(), stronger->TypeAsString()));

	auto [lhs, rhs] = lhsIsWeak ? std::tie(weaker, stronger) : std::tie(stronger, weaker);

	switch (stronger->Type()) {

	case t_undefined:
		return { lhs, rhs, CProgramRuntime::AcquireNewValue(), lhsIsWeak };
	case t_boolean:
		return { lhs, rhs, CProgramRuntime::AcquireNewBooleanValue(weaker->ToBoolean()), lhsIsWeak };
	case t_int:
		return { lhs, rhs, CProgramRuntime::AcquireNewIntValue(weaker->ToInt()), lhsIsWeak };
	case t_double:
		return { lhs, rhs, CProgramRuntime::AcquireNewDoubleValue(weaker->ToDouble()), lhsIsWeak };
	case t_string:
		break;
	}

	assert(false);
	return { lhs, rhs, nullptr, lhsIsWeak };
}
