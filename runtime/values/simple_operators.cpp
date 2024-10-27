#include "simple_operators.hpp"
#include "simple.hpp"
#include "integer.hpp"
#include "double.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"

//IValue* OP_ADDITION(IValue* lhs, IValue* rhs)
//{
//
//}
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
			return v.undefinedValue;

		case t_int:
			v.intValue = CProgramRuntime::AcquireNewIntValue();
			*v.intValue = CIntValue(dynamic_cast<CIntValue*>(rhs)->m_iValue);
			lhs->SetValue(v.intValue);
			return v.intValue;

		case t_double:
			v.doubleValue = CProgramRuntime::AcquireNewDoubleValue();
			*v.doubleValue = CDoubleValue(dynamic_cast<CDoubleValue*>(rhs)->m_dValue);
			lhs->SetValue(v.doubleValue);
			return v.doubleValue;
	}

	return nullptr;
}
