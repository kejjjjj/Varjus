#include "stdjson.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"

#ifdef _MSC_VER
#pragma warning(push, 0)
#include "nlohmann/json.hpp"
#pragma warning(pop)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wconversion"
#include "nlohmann/json.hpp"
#pragma GCC diagnostic pop
#endif

using json = nlohmann::json;

FORWARD_DECLARE_METHOD(ParseJson);
FORWARD_DECLARE_METHOD(StringifyJson);

void CStdJsonValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("parse"), ParseJson, 1);
	receiver.AddMethod(VSL("stringify"), StringifyJson, 2);
}

static CArrayValue* ParseArray(CRuntimeContext* const ctx, const json& arr);
static CObjectValue* ParseObject(CRuntimeContext* const ctx, const json& obj);

IValue* ParseJsonRecursively(CRuntimeContext* const ctx, const json& js)
{
	using value_t = json::value_t;
	switch (js.type()) {
	case value_t::binary:
		assert(false);
		[[fallthrough]];
	case value_t::null:
	case value_t::discarded:
		return IValue::Construct(ctx->m_pRuntime);
	case value_t::boolean:
		return CBooleanValue::Construct(ctx->m_pRuntime, js.get<json::boolean_t>());
	case value_t::number_integer:
		return CIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusInt>(js.get<json::number_integer_t>()));
	case value_t::number_unsigned:
		return CUIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusUInt>(js.get<json::number_unsigned_t>()));
	case value_t::number_float:
		return CDoubleValue::Construct(ctx->m_pRuntime, js.get<json::number_float_t>());
	case value_t::string:
#ifdef UNICODE
		return CStringValue::Construct(ctx->m_pRuntime, LocaleConverter::utf8_to_u16string(js.get<json::string_t>()));
#else
		return CStringValue::Construct(ctx->m_pRuntime, js.get<json::string_t>());
#endif
	case value_t::array:
		return ParseArray(ctx, js);
	case value_t::object:
		return ParseObject(ctx, js);
	default:
		assert(false);
		return nullptr;
	}

}

CArrayValue* ParseArray(CRuntimeContext* const ctx, const json& arr)
{
	IValues values;

	for (const auto& v : arr) {
		values.push_back(ParseJsonRecursively(ctx, v));
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(values));
}
CObjectValue* ParseObject(CRuntimeContext* const ctx, const json& obj)
{
	ObjectValues properties;

	for (const auto& [k, v] : obj.items()) {
#ifdef UNICODE
		auto key = CStringValue::Construct(ctx->m_pRuntime, LocaleConverter::utf8_to_u16string(k));
#else
		auto key = CStringValue::Construct(ctx->m_pRuntime, k);
#endif
		auto value = ParseJsonRecursively(ctx, v);

		properties.push_back({ key, value });

	}

	return CObjectValue::Construct(ctx->m_pRuntime, std::move(properties));
}

DEFINE_METHOD(ParseJson, args)
{
	const auto str = args[0];

	if (str->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("json.parse expected a string, but got \"{}\""), str->TypeAsString()));
	}

	try {
		const auto data = json::parse(str->ToString());
		return ParseJsonRecursively(ctx, data);
	}
	catch (json::parse_error& err) {
		//let the user catch it
		ctx->m_pRuntime->GetExceptionValue() = CStringValue::Construct(ctx->m_pRuntime, fmt::format(VSL("{}"), err.what()));
		ctx->m_pRuntime->ThrowException();
		return ctx->m_pRuntime->GetExceptionValue();
	}

}

DEFINE_METHOD(StringifyJson, args)
{
	const auto obj = args[0];
	const auto pretty = args[1];

	if (obj->Type() != t_object) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("json.stringify expected an object, but got \"{}\""), obj->TypeAsString()));
	}
	if (!pretty->IsBooleanConvertible()) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("json.stringify 2. expected a boolean, but got \"{}\""), pretty->TypeAsString()));
	}

	if(pretty->ToBoolean())
		return CStringValue::Construct(ctx->m_pRuntime, obj->Dump());

	return CStringValue::Construct(ctx->m_pRuntime, obj->ValueAsString());
}