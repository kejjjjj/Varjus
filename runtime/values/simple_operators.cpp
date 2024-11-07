#include "coercion.hpp"
#include "simple_operators.hpp"

#include "types/types.hpp"
#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

IValue* OP_ASSIGNMENT(IValue* lhs, IValue* rhs)
{
	auto variable = lhs->GetOwner();
	if (!variable)
		throw CRuntimeError("Left-handside must have a memory address");

	if (lhs->IsImmutable())
		throw CRuntimeError("Cannot assign to an immutable value");

	if (lhs == rhs)
		return lhs;

	if (auto value = variable->GetValue()) {
		value->SetOwner(nullptr);
		value->Release();
	}

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
			variable->SetValue(CProgramRuntime::AcquireNewValue<CStringValue>(rhs->AsString()));
			break;
		case t_callable:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CCallableValue>(rhs->AsCallable()));
			break;
	}

	lhs->SetOwner(variable);
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
		result = CProgramRuntime::AcquireNewValue<CStringValue>(lhs->ToString() + rhs->ToString());
		break;
	case t_callable:
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
		assert(false);
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}