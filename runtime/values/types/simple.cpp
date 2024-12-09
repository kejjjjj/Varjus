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
	return ToCBoolean()->Get(); 
}
std::int64_t& IValue::AsInt() {
	return ToCInt()->Get();
}
double& IValue::AsDouble() {
	return ToCDouble()->Get();
}
std::string& IValue::AsString(){
	return ToCString()->Internal()->GetString();
}

std::string IValue::ToPrintableString() const
{
	return std::format("{}: {}", ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]]std::int64_t index)
{
	return nullptr;
}




