#include "opt_operators.hpp"
#include "opt_coercion.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/error.hpp"

#define MU [[maybe_unused]]
IConstEvalValue* OPT_ASSIGN(MU IConstEvalValue* lhs, MU IConstEvalValue* rhs)
{
	return nullptr;
}

IConstEvalValue* OPT_ADDITION(IConstEvalValue* _lhs, IConstEvalValue* _rhs)
{

	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IConstEvalValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = COptimizationValues::AcquireNewValue<CConstEvalIntValue>(lhs->ToInt() + rhs->ToInt());
		break;
	case t_boolean:
	case t_double:
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		CLinterErrors::PushError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;

}