#include "types.hpp"
#include "api/internal/runtime.hpp"

#include <format>

IValue* IValue::Construct() {
	return CProgramRuntime::AcquireNewValue<IValue>();
}
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
	return ToCBoolean()->Get(); 
}
VarjusInt& IValue::AsInt() {
	return ToCInt()->Get();
}
VarjusUInt& IValue::AsUInt() {
	return ToCUInt()->Get();
}
VarjusDouble& IValue::AsDouble() {
	return ToCDouble()->Get();
}
std::string& IValue::AsString(){
	return ToCString()->Internal()->GetString();
}

std::string IValue::ToPrintableString() const
{
	return std::format("{}: {}", ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]]IValue* index)
{
	assert(false);
	return nullptr;
}

