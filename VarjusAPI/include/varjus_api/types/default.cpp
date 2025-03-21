#include "types.hpp"
#include "varjus_api/internal/runtime.hpp"




IValue* IValue::Construct(CProgramRuntime* const runtime) {
	return runtime->AcquireNewValue<IValue>();
}
void IValue::ReleaseInternal()
{
	RemoveConstness();
	SetOwner(nullptr);
}
void IValue::Release()
{
	ReleaseInternal();
	m_pAllocator->FreeValue<IValue>(this);
}
IValue* IValue::Copy()
{
	return m_pAllocator->AcquireNewValue<IValue>();
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
VarjusString& IValue::AsString(){
	return ToCString()->Internal()->GetString();
}

VarjusString IValue::ToPrintableString() const
{
	return fmt::format(VSL("{}: {}"), ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]]IValue* index)
{
	assert(false);
	return nullptr;
}

