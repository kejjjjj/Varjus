#include "opt_coercion.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/error.hpp"

#include <cassert>
#include <format>

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
		return { nullptr, nullptr, nullptr, lhsIsWeak };

	auto [lhs, rhs] = lhsIsWeak ? std::tie(weaker, stronger) : std::tie(stronger, weaker);

	switch (stronger->Type()) {
	case t_boolean:
		return { lhs, rhs, COptimizationValues::AcquireNewValue<CConstEvalBooleanValue>(weaker->ToBoolean()), lhsIsWeak };
	case t_int:
		return { lhs, rhs, COptimizationValues::AcquireNewValue<CConstEvalIntValue>(weaker->ToInt()), lhsIsWeak };
	case t_double:
		return { lhs, rhs, COptimizationValues::AcquireNewValue<CConstEvalDoubleValue>(weaker->ToDouble()), lhsIsWeak };
	case t_undefined:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		break;
	}

	return { nullptr, nullptr, nullptr, lhsIsWeak };
}
