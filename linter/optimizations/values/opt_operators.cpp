#include "opt_operators.hpp"
#include "opt_coercion.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/error.hpp"

#include <format>


#define MU [[maybe_unused]]
IConstEvalValue* OPT_ASSIGN(MU IConstEvalValue* lhs, MU IConstEvalValue* rhs)
{
	return nullptr;
}

IConstEvalValue* OPT_ADDITION(IConstEvalValue* _lhs, IConstEvalValue* _rhs)
{

	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);

	if (!lhs || !rhs)
		return nullptr;

	IConstEvalValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = COptimizationValues::AcquireNewValue<CConstEvalBooleanValue>(static_cast<bool>(lhs->ToBoolean() + rhs->ToBoolean()));
		break;
	case t_int:
		result = COptimizationValues::AcquireNewValue<CConstEvalIntValue>(lhs->ToInt() + rhs->ToInt());
		break;
	case t_double:
		result = COptimizationValues::AcquireNewValue<CConstEvalDoubleValue>(lhs->ToDouble() + rhs->ToDouble());
		break;
	case t_string:
		result = COptimizationValues::AcquireNewValue<CConstEvalStringValue>(lhs->ToString() + rhs->ToString());
		break;
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;

}