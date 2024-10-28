#include "simple.hpp"
#include "integer.hpp"
#include "double.hpp"
#include "boolean.hpp"

#include "runtime/runtime.hpp"

#include <format>
#include <iostream>

void IValue::Release()
{
	switch (Type()) {
	case t_undefined:
		return CProgramRuntime::FreeUndefinedValue(this);
	case t_boolean:
		return CProgramRuntime::FreeIntValue(dynamic_cast<CIntValue*>(this));
	case t_int:
		return CProgramRuntime::FreeIntValue(dynamic_cast<CIntValue*>(this));
	case t_double:
		return CProgramRuntime::FreeDoubleValue(dynamic_cast<CDoubleValue*>(this));
	}
}
bool IValue::AsBoolean() const{
	return dynamic_cast<const CBooleanValue*>(this)->m_bValue;
}
std::int64_t IValue::AsInt() const{
	return dynamic_cast<const CIntValue*>(this)->m_iValue;
}
double IValue::AsDouble() const {
	return dynamic_cast<const CDoubleValue*>(this)->m_dValue;
}
void IValue::Print() const
{
	std::cout << std::format("{}: {}", ValueAsString(), TypeAsString()) << '\n';
}
