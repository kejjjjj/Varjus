#include "types.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/types/internal/object_declarations.hpp"
#include "linter/context.hpp"

using namespace Varjus;

IValue* IValue::Construct(Varjus::CProgramRuntime* const runtime) {
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
	return Varjus::fmt::format(VSL("{}: {}"), ValueAsString(), TypeAsString());
}

IValue* IValue::Index([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] IValue* index)
{
	assert(false);
	return nullptr;
}

static VarjusString DumpArray(VarjusUInt indent, VarjusChar indentChar, const CArrayValue* arr);
static VarjusString DumpObject(VarjusUInt indent, VarjusChar indentChar, const CObjectValue* obj);

VarjusString IValue::Dump(VarjusUInt indent, VarjusChar indentChar) const
{
	VarjusString ss;

	const auto write = [&ss](const VarjusString& v) { ss += v; };

	VarjusString indent_string(indent, indentChar);

	switch (Type()) {
	case t_undefined:
	case t_boolean:
	case t_uint:
	case t_int:
	case t_double:
	case t_string:
	case t_callable:
		write(ValueAsString());
		break;
	case t_array:
		write(DumpArray(indent, indentChar, ToArray()));
		break;
	case t_object:
		write(DumpObject(indent, indentChar, ToObject()));
		break;
	default:
		assert(false);
	}

	return ss;
}

VarjusString DumpArray(VarjusUInt indent, VarjusChar indentChar, const CArrayValue* arr)
{
	VarjusString ss;

	for (auto& v : arr->Internal()->GetContent().GetVariables()) {
		const auto str = v->GetValue() == arr ? VSL("...") : v->GetValue()->Dump(indent, indentChar);
		ss += str + VSL(", ");
	}

	if (ss.empty())
		return VSL("[]");

	ss.erase(ss.size() - 2, 2); //remove ", "
	return VarjusChar('[') + ss + VarjusChar(']');
}

VarjusString DumpObject(VarjusUInt indent, VarjusChar indentChar, const CObjectValue* obj)
{
	if (const auto bobj = obj->ToBuiltInObject())
		return DumpBuiltInObject(indent, indentChar, bobj);

	VarjusString ss;
	const auto write = [&ss](const VarjusString& v) { ss += v; };

	VarjusString indent_string(indent, indentChar);


	if (obj->GetShared()->GetAggregateValue().Iterator().empty()) {
		write(VSL("{}"));
		return ss;
	}

	write(VSL("{\n"));


	for (const auto& [key, value] : obj->GetShared()->GetAggregateValue().Iterator()) {

		write(indent_string);
		write(VSL("\"") + obj->GetAllocator()->KeyToString(key) + VSL("\""));
		write(VSL(": "));
		write(value->GetValue()->Dump(indent + 1, indentChar));
		write(VSL(",\n"));
	}

	//remove ,\n
	ss.erase(ss.size() - 2, 2);

	write(VSL("\n"));
	write(indent_string);
	write(VSL("}"));

	return ss;
}
VarjusString Varjus::DumpBuiltInObject(VarjusUInt indent, VarjusChar indentChar, const CBuiltInObject* obj)
{
	VarjusString ss;
	const auto write = [&ss](const VarjusString& v) { ss += v; };

	VarjusString indent_string(indent, indentChar);

	if (obj->m_oMethods->empty() && obj->m_oProperties->empty()) {
		write(VSL("{}"));
		return ss;
	}

	write(VSL("{\n"));

	const auto& methodInfo = obj->m_oMethods->m_pInfo;

	for (const auto& [id, _] : *obj->m_oMethods) {
		write(indent_string);
		write(VSL("\"") + methodInfo->m_oAllMembers.At(id) + VSL("\""));
		write(VSL(": \"method\",\n"));
	}

	const auto& propertyInfo = obj->m_oProperties->m_pInfo;

	for (const auto& [id, _] : *obj->m_oMethods) {
		write(indent_string);
		write(VSL("\"") + propertyInfo->m_oAllMembers.At(id) + VSL("\""));
		write(VSL(": \"property\",\n"));
	}

	//remove ,\n
	ss.erase(ss.size() - 2, 2);

	write(VSL("\n"));
	write(indent_string);
	write(VSL("}"));

	return ss;

}
