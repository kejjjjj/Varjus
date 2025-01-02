
#ifdef OPTIMIZATIONS

#include "opt_operators.hpp"
#include "opt_coercion.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/error.hpp"

#include <format>

#define DEFINE_CONSTEXPR_OPERATOR(name)\
IConstEvalValue* name(IConstEvalValue* _lhs, IConstEvalValue* _rhs)


DEFINE_CONSTEXPR_OPERATOR(OPT_ASSIGN)
{

	if (!_lhs->HasOwner())
		return nullptr;

	if (_lhs == _rhs)
		return _lhs;

	auto variable = _lhs->GetOwner();

	switch (_rhs->Type()) {
	case t_undefined:
		variable->SetValue(COptimizationValues::AcquireNewValue<IConstEvalValue>());
		break;
	case t_boolean:
		variable->SetValue(COptimizationValues::AcquireNewValue<CConstEvalBooleanValue>(_rhs->ToBoolean()));
		break;
	case t_int:
		variable->SetValue(COptimizationValues::AcquireNewValue<CConstEvalIntValue>(_rhs->ToInt()));
		break;
	case t_double:
		variable->SetValue(COptimizationValues::AcquireNewValue<CConstEvalDoubleValue>(_rhs->ToDouble()));
		break;
	case t_string:
		variable->SetValue(COptimizationValues::AcquireNewValue<CConstEvalStringValue>(_rhs->ToString()));
		break;
	case t_callable:
	case t_array:
	case t_object:
	default:
		return nullptr;
	}

	_lhs->SetOwner(variable);
	return variable->GetValue();
}

DEFINE_CONSTEXPR_OPERATOR(OPT_ADDITION)
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

#endif