#include "coercion.hpp"
#include "simple_operators.hpp"

#include "types/types.hpp"
#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* OP_ASSIGNMENT(IValue* lhs, IValue* rhs)
{
	auto variable = lhs->GetOwner();

	if (lhs->IsImmutable())
		throw CRuntimeError("cannot assign to an immutable value");

	if (!variable)
		throw CRuntimeError("cannot assign to a temporary value");

	assert(lhs != rhs);
	if (lhs == rhs)
		return lhs;

	switch (rhs->Type()) {
		case t_undefined:
			variable->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
			break;
		case t_boolean:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CBooleanValue>(rhs->AsBoolean()));
			break;
		case t_int:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CIntValue>(rhs->AsInt()));
			break;
		case t_double:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CDoubleValue>(rhs->AsDouble()));
			break;
		case t_string:
			variable->SetValue(CStringValue::Construct(rhs->AsString()));
			break;
		case t_callable:
			assert(rhs->ToCallable());
			variable->SetValue(rhs->ToCallable()->Copy());
			break;
		case t_array:
			assert(rhs->ToArray());
			variable->SetValue(rhs->ToArray()->Copy());
			break;
		case t_object:
			assert(rhs->ToObject());
			variable->SetValue(rhs->ToObject()->Copy());
			break;
		case t_builtin_member_callable:
			assert(rhs->ToBuiltInMemberCallable());
			variable->SetValue(rhs->ToBuiltInMemberCallable()->Copy());
			break;
	}

	lhs->SetOwner(variable);

	if (!lhs->GetOwner())
		throw CRuntimeError("lhs lost owner");

	return lhs;
}

IValue* OP_ADDITION(IValue* _lhs, IValue* _rhs)
{
	if (_lhs->Type() == t_undefined || _rhs->Type() == t_undefined) {
		throw CRuntimeError("incompatible operands");
	}

	//lhs contains the weaker value that possibly needs to be freed
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);

	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		break;
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(static_cast<bool>(lhs->ToBoolean() + rhs->ToBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() + rhs->ToInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(lhs->ToDouble() + rhs->ToDouble());
		break;
	case t_string:
		result = CStringValue::Construct(lhs->ToString() + rhs->ToString());
		break;
	case t_callable:
	case t_array:
	case t_object:
	case t_builtin_member_callable:
		assert(false);
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_LESS_THAN(IValue* _lhs, IValue* _rhs)
{
	if (_lhs->Type() == t_undefined || _rhs->Type() == t_undefined) {
		throw CRuntimeError("incompatible operands");
	}

	//lhs contains the weaker value that possibly needs to be freed
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);


	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		break;
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() < rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() < rhs->AsInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() < rhs->AsDouble());
		break;
	case t_string:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->ToString().length() < rhs->ToString().length());
		break;
	case t_callable:
	case t_array:
	case t_object:
	case t_builtin_member_callable:
		assert(false);
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}