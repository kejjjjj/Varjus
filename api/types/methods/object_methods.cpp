#include "api/types/types.hpp"

#include "linter/context.hpp"

#include "api/internal/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "api/internal/structure.hpp"


[[nodiscard]] CObjectValue* GetThisObject(IValue* _this) {
	return _this->ToObject();
}

FORWARD_DECLARE_METHOD(Object_Keys);
FORWARD_DECLARE_METHOD(Object_Values);
FORWARD_DECLARE_METHOD(Object_Set);
FORWARD_DECLARE_METHOD(Object_Remove);
FORWARD_DECLARE_METHOD(Object_Contains);
FORWARD_DECLARE_METHOD(Object_ToArray);

std::unique_ptr<BuiltInMethod_t> CObjectValue::ConstructMethods()
{
	auto m_oMethods = std::make_unique<BuiltInMethod_t>();

	m_oMethods->AddMethod("keys",     Object_Keys,     0u);
	m_oMethods->AddMethod("values",   Object_Values,   0u);
	m_oMethods->AddMethod("set",      Object_Set,      2u);
	m_oMethods->AddMethod("remove",   Object_Remove,   1u);
	m_oMethods->AddMethod("contains", Object_Contains, 1u);
	m_oMethods->AddMethod("to_array", Object_ToArray,  0u);

	return m_oMethods;
}

FORWARD_DECLARE_PROPERTY(ObjectLength);
std::unique_ptr<BuiltInProperty_t> CObjectValue::ConstructProperties()
{
	auto m_oProperties = std::make_unique<BuiltInProperty_t>();
	m_oProperties->AddProperty("length", ObjectLength);

	return m_oProperties;
}

DEFINE_PROPERTY(ObjectLength) {
	auto __this = GetThisObject(_this);
	return CUIntValue::Construct(runtime, static_cast<VarjusUInt>(__this->Internal()->GetAggregateValue().Length()));
}


DEFINE_METHOD(Object_Keys, args)
{
	auto __this = GetThisObject(_this);
	auto& vars = __this->GetShared()->GetAggregateValue().Iterator();

	IValues results(vars.size());

	//result array
	for (auto i = std::size_t(0); const auto& [index, var] : vars) {
		results[i++] = CStringValue::Construct(ctx->m_pRuntime, CFileContext::m_oAllMembers.At(index));
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(results));
}
DEFINE_METHOD(Object_Values, args)
{
	auto __this = GetThisObject(_this);
	auto& vars = __this->GetShared()->GetAggregateValue().Iterator();

	IValues results(vars.size());

	//result array
	for (auto i = std::size_t(0); const auto & [index, var] : vars) {
		results[i++] = var->GetValue()->Copy();
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(results));
}

static auto GetAttribute(CObjectValue* obj, IValue* const key)
{
	const auto& aggregate = obj->Internal()->GetAggregateValue();
	return aggregate.Get(key->ValueAsString());
}
static void AddAttribute(CObjectValue* obj, IValue* const key, IValue* value)
{
	auto& aggregate = obj->Internal()->GetAggregateValue();

	//insert the member if necessary
	auto var = aggregate.AddAttribute(CFileContext::m_oAllMembers[key->ValueAsString()]);
	var->SetValue(value->Copy());
}

DEFINE_METHOD(Object_Set, args)
{
	auto __this = GetThisObject(_this);

	const auto& key = args[0];
	auto& value = args[1];
	

	//known key
	if (auto objKey = GetAttribute(__this, key)) {
		auto var = objKey->GetOwner();
		var->SetValue(value->Copy());
	} else {
		 AddAttribute(__this, key, value);
	}

	return __this->Copy();
}
DEFINE_METHOD(Object_Remove, args)
{
	auto __this = GetThisObject(_this);

	const auto& key = args[0];
	auto& aggregate = __this->Internal()->GetAggregateValue();
	auto keyStr = key->ValueAsString();

	if(!CFileContext::m_oAllMembers.Contains(keyStr))
		return CBooleanValue::Construct(ctx->m_pRuntime, false);

	return CBooleanValue::Construct(ctx->m_pRuntime, aggregate.RemoveAttribute(CFileContext::m_oAllMembers.At(keyStr)));
}

static auto Contains(CObjectValue* obj, IValue* const key)
{
	const auto& aggregate = obj->Internal()->GetAggregateValue();
	return aggregate.Contains(key->ValueAsString());
}
DEFINE_METHOD(Object_Contains, args)
{
	auto __this = GetThisObject(_this);
	return CBooleanValue::Construct(ctx->m_pRuntime, Contains(__this, args[0]));
}
DEFINE_METHOD(Object_ToArray, args)
{
	auto __this = GetThisObject(_this);

	const auto& aggregate = __this->Internal()->GetAggregateValue().Iterator();

	IValues values;

	for (const auto& [i, var] : aggregate) {

		auto key = CStringValue::Construct(ctx->m_pRuntime, CFileContext::m_oAllMembers.At(i));
		auto value = var->GetValue()->Copy();

		values.push_back(CArrayValue::Construct(ctx->m_pRuntime, { key, value }));
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(values));
}