#include "types.hpp"
#include "runtime/runtime.hpp"

#include <format>
#include <iostream>

void IValue::Release()
{
	switch (Type()) {
	case t_undefined:
		return CProgramRuntime::FreeUndefinedValue(this);
	case t_boolean:
		return CProgramRuntime::FreeBooleanValue(dynamic_cast<CBooleanValue*>(this));
	case t_int:
		return CProgramRuntime::FreeIntValue(dynamic_cast<CIntValue*>(this));
	case t_double:
		return CProgramRuntime::FreeDoubleValue(dynamic_cast<CDoubleValue*>(this));
	}
}
IValue* IValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue();
}
bool& IValue::AsBoolean(){
	return dynamic_cast<CBooleanValue*>(this)->m_bValue;
}
std::int64_t& IValue::AsInt() {
	return dynamic_cast<CIntValue*>(this)->m_iValue;
}
double& IValue::AsDouble() {
	return dynamic_cast<CDoubleValue*>(this)->m_dValue;
}
void IValue::Print() const
{
	std::cout << std::format("{}: {}", ValueAsString(), TypeAsString()) << '\n';
}
