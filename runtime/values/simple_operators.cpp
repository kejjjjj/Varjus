#include "coercion.hpp"
#include "simple_operators.hpp"

#include "types/types.hpp"
#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

void OP_ASSIGNMENT(IValue* lhs, IValue* rhs)
{
	auto variable = lhs->GetOwner();
	if (!variable)
		throw CRuntimeError("Left-handside must have a memory address");

	if (lhs->IsImmutable())
		throw CRuntimeError("Cannot assign to an immutable value");

	if (lhs == rhs)
		return;

	if (auto value = variable->GetValue()) {
		value->SetOwner(nullptr);
		value->Release();
	}

	switch (rhs->Type()) {
		case t_undefined:
			variable->SetValue(CProgramRuntime::AcquireNewValue());
			break;
		case t_boolean:
			variable->SetValue(CProgramRuntime::AcquireNewBooleanValue(rhs->AsBoolean()));
			break;
		case t_int:
			variable->SetValue(CProgramRuntime::AcquireNewIntValue(rhs->AsInt()));
			break;
		case t_double:
			variable->SetValue(CProgramRuntime::AcquireNewDoubleValue(rhs->AsDouble()));
			break;
		case t_string:
			variable->SetValue(CProgramRuntime::AcquireNewStringValue(rhs->AsString()));
			break;
	}

	lhs->SetOwner(variable);
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
		result = CProgramRuntime::AcquireNewBooleanValue(static_cast<bool>(lhs->ToBoolean() + rhs->ToBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewIntValue(lhs->ToInt() + rhs->ToInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewDoubleValue(lhs->ToDouble() + rhs->ToDouble());
		break;
	case t_string:
		result = CProgramRuntime::AcquireNewStringValue(lhs->ToString() + rhs->ToString());
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
		result = CProgramRuntime::AcquireNewBooleanValue(lhs->AsBoolean() < rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewBooleanValue(lhs->AsInt() < rhs->AsInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewBooleanValue(lhs->AsDouble() < rhs->AsDouble());
		break;
	case t_string:
		result = CProgramRuntime::AcquireNewBooleanValue(lhs->ToString().length() < rhs->ToString().length());
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}