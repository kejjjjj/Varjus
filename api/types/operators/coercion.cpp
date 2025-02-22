#include "coercion.hpp"

#include "api/types/types.hpp"
#include "api/internal/runtime.hpp"

#include "runtime/exceptions/exception.hpp"

#include <cassert>
#include <format>

std::tuple<IValue*, IValue*, IValue*> Coerce(IValue* lhs, IValue* rhs)
{
	if (lhs->Type() == rhs->Type()) {
		return { lhs, rhs, nullptr };
	}

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

	if (!weaker->IsCoerceable() || !stronger->IsCoerceable())
		throw CRuntimeError(std::format("cannot coerce from \"{}\" to \"{}\"", weaker->TypeAsString(), stronger->TypeAsString()));

	auto [lhs, rhs] = lhsIsWeak ? std::tie(weaker, stronger) : std::tie(stronger, weaker);

	switch (stronger->Type()) {

	case t_undefined:
		return { lhs, rhs, IValue::Construct(), lhsIsWeak};
	case t_boolean:
		return { lhs, rhs, CBooleanValue::Construct(weaker->ToBoolean()), lhsIsWeak };
	case t_int:
		return { lhs, rhs, CIntValue::Construct(weaker->ToInt()), lhsIsWeak };
	case t_uint:
		return { lhs, rhs, CUIntValue::Construct(weaker->ToUInt()), lhsIsWeak };
	case t_double:
		return { lhs, rhs, CDoubleValue::Construct(weaker->ToDouble()), lhsIsWeak };
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		break;
	}

	assert(false);
	return { lhs, rhs, nullptr, lhsIsWeak };
}
