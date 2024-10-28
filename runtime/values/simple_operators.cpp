#include "simple_operators.hpp"
#include "simple.hpp"
#include "integer.hpp"
#include "double.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"

IValue* OP_ASSIGNMENT(CVariable* lhs, IValue* rhs)
{
	if (!lhs)
		throw std::exception("Left-handside must have a memory address");

	union {
		IValue* undefinedValue;
		CIntValue* intValue;
		CDoubleValue* doubleValue;
	}v{};

	switch (rhs->Type()) {
		case t_undefined:
			v.undefinedValue = CProgramRuntime::AcquireNewValue();
			lhs->SetValue(v.undefinedValue);
			break;

		case t_int:
			v.intValue = CProgramRuntime::AcquireNewIntValue();
			*v.intValue = CIntValue(rhs->AsInt());
			lhs->SetValue(v.intValue);
			break;

		case t_double:
			v.doubleValue = CProgramRuntime::AcquireNewDoubleValue();
			*v.doubleValue = CDoubleValue(rhs->AsDouble());
			lhs->SetValue(v.doubleValue);
			break;
	}

	return lhs->GetValue();
}

IValue* OP_ADDITION(IValue* lhs, IValue* rhs)
{
	if (lhs->Type() != t_int || rhs->Type() != lhs->Type()) {
		throw std::exception("incompatible operands");
	}

	union {
		IValue* undefinedValue;
		CIntValue* intValue;
		CDoubleValue* doubleValue;
	}v{};

	switch (lhs->Type()) {
	case t_undefined:
		return nullptr;

	case t_int:
		v.intValue = CProgramRuntime::AcquireNewIntValue();
		*v.intValue = CIntValue(lhs->AsInt() + rhs->AsInt());
		return v.intValue;

	case t_double:
		v.doubleValue = CProgramRuntime::AcquireNewDoubleValue();
		*v.doubleValue = CDoubleValue(lhs->AsDouble() + rhs->AsDouble());
		return v.doubleValue;
	}

	return nullptr;
}
