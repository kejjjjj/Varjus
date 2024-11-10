#include "types.hpp"
#include "runtime/runtime.hpp"

#include <format>

void IValue::ReleaseInternal()
{
	RemoveConstness();
	SetOwner(nullptr);
}
void IValue::Release()
{
	ReleaseInternal();
	return CProgramRuntime::FreeValue<IValue>(this);
}
IValue* IValue::Copy()
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
CRuntimeFunction* IValue::AsCallable()
{
	return dynamic_cast<CCallableValue*>(this)->m_oValue;
}
CInternalArrayValue* IValue::AsArray()
{
	return dynamic_cast<CInternalArrayValue*>(this);
}
std::string IValue::ToPrintableString() const
{
	return std::format("{}: {}", ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]]std::int64_t index)
{
	return nullptr;
}




