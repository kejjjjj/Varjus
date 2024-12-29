#include "opt_coercion.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/error.hpp"

#include <cassert>


std::tuple<IConstEvalValue*, IConstEvalValue*, IConstEvalValue*> Coerce(IConstEvalValue* lhs, IConstEvalValue* rhs)
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
CCoercionOperands CoerceInternal(IConstEvalValue* weaker, IConstEvalValue* stronger, bool lhsIsWeak)
{
	assert(weaker->Type() != stronger->Type());

	if (!weaker->IsCoerceable() || !stronger->IsCoerceable())
		throw CLinterError(std::format("cannot coerce from \"{}\" to \"{}\"", weaker->TypeAsString(), stronger->TypeAsString()));

	auto [lhs, rhs] = lhsIsWeak ? std::tie(weaker, stronger) : std::tie(stronger, weaker);

	switch (stronger->Type()) {
	case t_int:
		return { lhs, rhs, COptimizationValues::AcquireNewValue<CConstEvalIntValue>(weaker->ToInt()), lhsIsWeak };
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		break;
	}

	assert(false);
	return { lhs, rhs, nullptr, lhsIsWeak };
}
