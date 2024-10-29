#include "coercion.hpp"
#include "simple_operators.hpp"

#include "types/types.hpp"
#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"

void OP_ASSIGNMENT(IValue* lhs, IValue* rhs)
{
	auto variable = lhs->GetOwner();
	if (!variable)
		throw std::exception("Left-handside must have a memory address");

	IVALUE_UNION v{};

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
	}

	lhs->SetOwner(variable);
}

IValue* OP_ADDITION(IValue* _lhs, IValue* _rhs)
{
	if (_lhs->Type() == t_undefined || _rhs->Type() == t_undefined) {
		throw std::exception("incompatible operands");
	}

	//lhs contains the weaker value that possibly needs to be freed
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);

	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		break;
	case t_boolean:
		result = CProgramRuntime::AcquireNewBooleanValue(static_cast<bool>(lhs->AsBoolean() + rhs->AsBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewIntValue(lhs->AsInt() + rhs->AsInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewDoubleValue(lhs->AsDouble() + rhs->AsDouble());
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
		throw std::exception("incompatible operands");
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
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}