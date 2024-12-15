
#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/values/simple_operators.hpp"

#include <algorithm>

CStringValue::StringMethods CStringValue::ConstructMethods()
{
	return {
		{"toupper",    {0u, &CStringValue::ToUpper}},
		{"tolower",    {0u, &CStringValue::ToLower}},
		{"substring",  {2u, &CStringValue::Substring}},
		{"split",      {1u, &CStringValue::Split}},
		{"replace",    {2u, &CStringValue::Replace}},
		{"repeat",     {1u, &CStringValue::Repeat}},
		{"get_code_at",{1u, &CStringValue::GetCodeAt}},

	};


}

IValue* CStringValue::ToUpper([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValue)
{
	std::string v = Internal()->GetString();
	std::ranges::transform(v, v.begin(), [](std::int8_t c) { 
		return static_cast<std::int8_t>(std::toupper(static_cast<std::int32_t>(c))); });
	return Construct(v);
}
IValue* CStringValue::ToLower([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValue)
{
	std::string v = Internal()->GetString();
	std::ranges::transform(v, v.begin(), [](std::int8_t c) {
		return static_cast<std::int8_t>(std::tolower(static_cast<std::int32_t>(c))); });
	return Construct(v);
}
IValue* CStringValue::Substring([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValue)
{
	std::string& v = Internal()->GetString();
	
	auto& a = newValue[0];
	auto& b = newValue[1];

	const auto CheckSanity = [](const IValue* v) {
		if (!v->IsIntegral())
			throw CRuntimeError(std::format("string.substring expected an integral value, but got \"{}\"", v->TypeAsString()));
	};

	CheckSanity(a);
	CheckSanity(b);

	auto start = a->ToInt();
	auto end = b->ToInt();

	const auto CheckRange = [&v](const auto value) {
		if(value < 0 || value >= static_cast<std::int64_t>(v.length()))
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

IValue* CStringValue::Split([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValue)
{
	std::string& v = Internal()->GetString();

	auto& delimiter = newValue.front();

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

IValue* CStringValue::Replace([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValue)
{
	std::string& v = Internal()->GetString();
	
	const auto CheckSanity = [](const IValue* v) {
		if (v->Type() != t_string)
			throw CRuntimeError(std::format("string.substring expected a \"string\", but got \"{}\"", v->TypeAsString()));
	};

	auto& a = newValue[0];
	auto& b = newValue[1];

	CheckSanity(a);
	CheckSanity(b);

	return CStringValue::Construct(ReplaceAll(v, a->AsString(), b->AsString()));
}

IValue* CStringValue::Repeat([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValue)
{
	std::string& v = Internal()->GetString();

	auto& countValue = newValue[0];
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

IValue* CStringValue::GetCodeAt([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValue)
{
	std::string& v = Internal()->GetString();

	auto& idx = newValue[0];
	if (!idx->IsIntegral())
		throw CRuntimeError(std::format("string.get_code_at expected an integral value, but got \"{}\"", idx->TypeAsString()));

	const auto index = idx->ToInt();

	if (index < 0 || index >= static_cast<std::int64_t>(v.length()))
		throw CRuntimeError("string.get_code_at index out of range");

	const auto asUnsigned = static_cast<std::size_t>(index);

	return CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<std::int64_t>(v[asUnsigned]));
}