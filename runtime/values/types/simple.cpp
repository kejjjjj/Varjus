#include "types.hpp"
#include "runtime/runtime.hpp"

#include <format>
#include <iostream>

void IValue::Release()
{
	m_bIsConst = false;

	switch (Type()) {
	case t_undefined:
		return CProgramRuntime::FreeValue(this);
	case t_boolean:
		return CProgramRuntime::FreeValue<CBooleanValue>(dynamic_cast<CBooleanValue*>(this));
	case t_int:
		return CProgramRuntime::FreeValue<CIntValue>(dynamic_cast<CIntValue*>(this));
	case t_double:
		return CProgramRuntime::FreeValue<CDoubleValue>(dynamic_cast<CDoubleValue*>(this));
	case t_string:
		return CProgramRuntime::FreeValue<CStringValue>(dynamic_cast<CStringValue*>(this));
	case t_callable:
		assert(false);
		break;
	}
}
IValue* IValue::Copy() const
{
	return CProgramRuntime::AcquireNewValue<IValue>();
}
bool& IValue::AsBoolean(){
	return dynamic_cast<CBooleanValue*>(this)->m_oValue;
}
std::int64_t& IValue::AsInt() {
	return dynamic_cast<CIntValue*>(this)->m_oValue;
}
double& IValue::AsDouble() {
	return dynamic_cast<CDoubleValue*>(this)->m_oValue;
}
std::string& IValue::AsString(){
	return dynamic_cast<CStringValue*>(this)->m_oValue;
}
std::string IValue::ToPrintableString() const
{
	return std::format("{}: {}", ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]]std::int64_t index)
{
	return nullptr;
}
