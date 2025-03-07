
#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/types/internal/object_declarations.hpp"

#include "runtime/exceptions/exception.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/structure.hpp"


#include <algorithm>
#include <ranges>

[[nodiscard]] CStringValue* GetThisString(IValue* _this) {
	return _this->ToCString();
}

FORWARD_DECLARE_METHOD(ToUpper);
FORWARD_DECLARE_METHOD(ToLower);

FORWARD_DECLARE_METHOD(Substring);
FORWARD_DECLARE_METHOD(Split);

FORWARD_DECLARE_METHOD(Replace);
FORWARD_DECLARE_METHOD(Reproduce);

FORWARD_DECLARE_METHOD(GetCodeAt);

std::unique_ptr<BuiltInMethod_t> CStringValue::ConstructMethods(CProgramInformation* const info)
{
	auto m_oMethods = std::make_unique<BuiltInMethod_t>(info);

	m_oMethods->AddMethod("toupper",     ToUpper,   0u);
	m_oMethods->AddMethod("tolower",     ToLower,   0u);
	m_oMethods->AddMethod("substring",   Substring, 2u);
	m_oMethods->AddMethod("split",       Split,     1u);
	m_oMethods->AddMethod("replace",     Replace,   2u);
	m_oMethods->AddMethod("reproduce",   Reproduce, 1u);
	m_oMethods->AddMethod("get_code_at", GetCodeAt, 1u);

	return m_oMethods;

}

FORWARD_DECLARE_PROPERTY(StringLength);

std::unique_ptr<BuiltInProperty_t> CStringValue::ConstructProperties(CProgramInformation* const info)
{
	auto m_oProperties = std::make_unique<BuiltInProperty_t>(info);
	m_oProperties->AddProperty("length", StringLength);

	return m_oProperties;
}

DEFINE_PROPERTY(StringLength) {
	auto __this = GetThisString(_this);
	return CUIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusUInt>(__this->Internal()->Length()));
}

DEFINE_METHOD(ToUpper, args)
{
	auto __this = GetThisString(_this);
	std::string v = __this->ToString();

	std::ranges::transform(v, v.begin(), [](std::int8_t c) { 
		return static_cast<std::int8_t>(std::toupper(static_cast<std::int32_t>(c))); });
	return CStringValue::Construct(ctx->m_pRuntime, v);
}
DEFINE_METHOD(ToLower, args)
{
	auto __this = GetThisString(_this);
	std::string v = __this->ToString();

	std::ranges::transform(v, v.begin(), [](std::int8_t c) {
		return static_cast<std::int8_t>(std::tolower(static_cast<std::int32_t>(c))); });
	return CStringValue::Construct(ctx->m_pRuntime, v);
}
DEFINE_METHOD(Substring, args)
{
	auto __this = GetThisString(_this);
	const auto& v = __this->ToString();

	auto& a = args[0];
	auto& b = args[1];

	const auto CheckSanity = [&ctx](const IValue* v) {
		if (!v->IsIntegral())
			throw CRuntimeError(ctx->m_pRuntime, std::format("string.substring expected an integral value, but got \"{}\"", v->TypeAsString()));
	};

	CheckSanity(a);
	CheckSanity(b);

	auto start = a->ToInt();
	auto end = b->ToInt();

	const auto CheckRange = [&v, &ctx](const auto value) {
		if(value < 0 || value >= static_cast<VarjusInt>(v.length()))
			throw CRuntimeError(ctx->m_pRuntime, "string.substring index out of range");
	};

	CheckRange(start);
	CheckRange(end);

	if(start >= end)
		throw CRuntimeError(ctx->m_pRuntime, "string.substring expected start < end");

	return CStringValue::Construct(ctx->m_pRuntime, v.substr(static_cast<std::size_t>(start), static_cast<std::size_t>(end)));
}

std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> result;
	std::size_t start{}, end{};

	if (delimiter.empty()) {
		for (char c : str) {
			result.push_back(std::string(1, c));
		}
		return result;
	}

	while ((end = str.find(delimiter, start)) != std::string::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
	}

	result.push_back(str.substr(start));
	return result;
}

DEFINE_METHOD(Split, args) {

	auto __this = GetThisString(_this);
	const auto& v = __this->ToString();

	auto& delimiter = args.front();

	if(delimiter->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, std::format("string.split expected a \"string\", but got \"{}\"", delimiter->TypeAsString()));

	auto tokens = SplitString(v, delimiter->AsString());
	IValues tokensAsValues;
	for (const auto& t : tokens)
		tokensAsValues.emplace_back(CStringValue::Construct(ctx->m_pRuntime, t));

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(tokensAsValues));
}
std::string ReplaceAll(const std::string& str, const std::string& oldSub, const std::string& newSub) {
	std::string result = str;
	size_t pos = 0;

	while ((pos = result.find(oldSub, pos)) != std::string::npos) {
		result.replace(pos, oldSub.length(), newSub);
		pos += newSub.length(); // move past the new substring to avoid infinite loops
	}

	return result;
}

DEFINE_METHOD(Replace, args) {

	auto __this = GetThisString(_this);
	const auto& v = __this->ToString();

	const auto CheckSanity = [&ctx](const IValue* v) {
		if (v->Type() != t_string)
			throw CRuntimeError(ctx->m_pRuntime, std::format("string.substring expected a \"string\", but got \"{}\"", v->TypeAsString()));
	};

	auto& a = args[0];
	auto& b = args[1];

	CheckSanity(a);
	CheckSanity(b);

	return CStringValue::Construct(ctx->m_pRuntime, ReplaceAll(v, a->AsString(), b->AsString()));
}

DEFINE_METHOD(Reproduce, args) {

	auto __this = GetThisString(_this);
	const auto& v = __this->ToString();

	auto& countValue = args[0];
	if (!countValue->IsIntegral())
		throw CRuntimeError(ctx->m_pRuntime, std::format("string.reproduce expected an integral value, but got \"{}\"", countValue->TypeAsString()));

	auto count = countValue->ToInt();
	if (count < 0)
		throw CRuntimeError(ctx->m_pRuntime, "string.reproduce expected count to be >= 0");

	std::string result;
	for ([[maybe_unused]] const auto i : std::views::iota(0ll, count))
		result += v;

	return CStringValue::Construct(ctx->m_pRuntime, result);
}

DEFINE_METHOD(GetCodeAt, args) {
	auto __this = GetThisString(_this);
	const auto& v = __this->ToString();

	auto& idx = args[0];
	if (!idx->IsIntegral())
		throw CRuntimeError(ctx->m_pRuntime, std::format("string.get_code_at expected an integral value, but got \"{}\"", idx->TypeAsString()));

	const auto index = idx->ToInt();

	if (index < 0 || index >= static_cast<VarjusInt>(v.length()))
		throw CRuntimeError(ctx->m_pRuntime, "string.get_code_at index out of range");

	const auto asUnsigned = static_cast<std::size_t>(index);

	return CIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusInt>(v[asUnsigned]));
}