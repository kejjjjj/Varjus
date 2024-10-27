#include "simple.hpp"
#include "integer.hpp"
#include "double.hpp"

#include "runtime/runtime.hpp"


void IValue::Release()
{
	switch (Type()) {
	case t_undefined:
		return CProgramRuntime::FreeUndefinedValue(this);
	case t_int:
		return CProgramRuntime::FreeIntValue(dynamic_cast<CIntValue*>(this));
	case t_double:
		return CProgramRuntime::FreeDoubleValue(dynamic_cast<CDoubleValue*>(this));
	}
}
