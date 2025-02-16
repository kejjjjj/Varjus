
#include "api/types/operators/default_operators.hpp"
#include "api/types/types.hpp"

#include "runtime/exceptions/exception.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"

#include <algorithm>


BuiltInMethod_t CStringValue::m_oMethods;
BuiltInProperty_t CStringValue::m_oProperties;

#define START_METHOD(name) \
if(!_this)\
	throw CRuntimeError("attempted to call a method without \"this\" context"); \
if(_this->Type() != t_string) \
	throw CRuntimeError(std::format("string.{} expected an array, but got {}", #name, _this->TypeAsString()));\
auto name = _this->ToCString()

FORWARD_DECLARE_METHOD(ToUpper);
FORWARD_DECLARE_METHOD(ToLower);

FORWARD_DECLARE_METHOD(Substring);
FORWARD_DECLARE_METHOD(Split);

FORWARD_DECLARE_METHOD(Replace);
FORWARD_DECLARE_METHOD(Repeat);

FORWARD_DECLARE_METHOD(GetCodeAt);

void CStringValue::ConstructMethods()
{
	m_oMethods.clear();

	m_oMethods.AddMethod("toupper",     ToUpper,   0u);
	m_oMethods.AddMethod("tolower",     ToLower,   0u);
	m_oMethods.AddMethod("substring",   Substring, 2u);
	m_oMethods.AddMethod("split",       Split,     1u);
	m_oMethods.AddMethod("replace",     Replace,   2u);
	m_oMethods.AddMethod("repeat",      Repeat,    1u);
	m_oMethods.AddMethod("get_code_at", GetCodeAt, 1u);
}

FORWARD_DECLARE_PROPERTY(StringLength);

void CStringValue::ConstructProperties()
{
	m_oProperties.clear();
	m_oProperties.AddProperty("length", StringLength);
}

DEFINE_PROPERTY(StringLength) {
	START_METHOD(__this);
	return CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<VarjusInt>(__this->Internal()->Length()));
}

DEFINE_METHOD(ToUpper)
{
	START_METHOD(__this);
	std::string v = __this->ToString();

	std::ranges::transform(v, v.begin(), [](std::int8_t c) { 
		return static_cast<std::int8_t>(std::toupper(static_cast<std::int32_t>(c))); });
	return CStringValue::Construct(v);
}
DEFINE_METHOD(ToLower) 
{
	START_METHOD(__this);
	std::string v = __this->ToString();

	std::ranges::transform(v, v.begin(), [](std::int8_t c) {
		return static_cast<std::int8_t>(std::tolower(static_cast<std::int32_t>(c))); });
	return CStringValue::Construct(v);
}
DEFINE_METHOD(Substring) 
{
	START_METHOD(__this);
	const auto& v = __this->ToString();

	auto& a = args[0];
	auto& b = args[1];

	const auto CheckSanity = [](const IValue* v) {
		if (!v->IsIntegral())
			throw CRuntimeError(std::format("string.substring expected an integral value, but got \"{}\"", v->TypeAsString()));
	};

	CheckSanity(a);
	CheckSanity(b);

	auto start = a->ToInt();
	auto end = b->ToInt();

	const auto CheckRange = [&v](const auto value) {
		if(value < 0 || value >= static_cast<VarjusInt>(v.length()))
			throw CRuntimeError("string.substring index out of range");
	};

	CheckRange(start);
	CheckRange(end);

	if(start >= end)
		throw CRuntimeError("string.substring expected start < end");

	return CStringValue::Construct(v.substr(static_cast<std::size_t>(start), static_cast<std::size_t>(end)));
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

DEFINE_METHOD(Split) {

	START_METHOD(__this);
	const auto& v = __this->ToString();

	auto& delimiter = args.front();

	if(delimiter->Type() != t_string)
		throw CRuntimeError(std::format("string.split expected a \"string\", but got \"{}\"", delimiter->TypeAsString()));

	auto tokens = SplitString(v, delimiter->AsString());
	IValues tokensAsValues;
	for (const auto& t : tokens)
		tokensAsValues.emplace_back(CStringValue::Construct(t));

	return CArrayValue::Construct(std::move(tokensAsValues));
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

DEFINE_METHOD(Replace) {

	START_METHOD(__this);
	const auto& v = __this->ToString();

	const auto CheckSanity = [](const IValue* v) {
		if (v->Type() != t_string)
			throw CRuntimeError(std::format("string.substring expected a \"string\", but got \"{}\"", v->TypeAsString()));
	};

	auto& a = args[0];
	auto& b = args[1];

	CheckSanity(a);
	CheckSanity(b);

	return CStringValue::Construct(ReplaceAll(v, a->AsString(), b->AsString()));
}

DEFINE_METHOD(Repeat) {

	START_METHOD(__this);
	const auto& v = __this->ToString();

	auto& countValue = args[0];
	if (!countValue->IsIntegral())
		throw CRuntimeError(std::format("string.repeat expected an integral value, but got \"{}\"", countValue->TypeAsString()));

	auto count = countValue->ToInt();
	if (count < 0)
		throw CRuntimeError("string.repeat expected count to be >= 0");

	std::string result;
	for ([[maybe_unused]] const auto i : std::views::iota(0ll, count))
		result += v;

	return CStringValue::Construct(result);
}

DEFINE_METHOD(GetCodeAt) {
	START_METHOD(__this);
	const auto& v = __this->ToString();

	auto& idx = args[0];
	if (!idx->IsIntegral())
		throw CRuntimeError(std::format("string.get_code_at expected an integral value, but got \"{}\"", idx->TypeAsString()));

	const auto index = idx->ToInt();

	if (index < 0 || index >= static_cast<VarjusInt>(v.length()))
		throw CRuntimeError("string.get_code_at index out of range");

	const auto asUnsigned = static_cast<std::size_t>(index);

	return CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<VarjusInt>(v[asUnsigned]));
}