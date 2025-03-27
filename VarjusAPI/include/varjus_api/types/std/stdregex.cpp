#include "stdregex.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"

#include <regex>

FORWARD_DECLARE_METHOD(RegexSearch);
FORWARD_DECLARE_METHOD(RegexMatch);
FORWARD_DECLARE_METHOD(RegexReplace);

void CStdRegex::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("search"),  RegexSearch,  2);
	receiver.AddMethod(VSL("match"),   RegexMatch,   2);
	receiver.AddMethod(VSL("replace"), RegexReplace, 3);


}

DEFINE_METHOD(RegexSearch, args)
{
	auto toMatch = args[0];
	auto target = args[1];

	if (toMatch->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.search 1. expected a string, but got \"{}\""), toMatch->TypeAsString()));

	if (target->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.search 2. expected a string, but got \"{}\""), target->TypeAsString()));


#ifdef UNICODE
	std::regex regex(LocaleConverter::u16string_to_ansi(toMatch->ToString()));
	auto targetStr = LocaleConverter::u16string_to_ansi(target->ToString());
#else
	std::regex regex(toMatch->ToString());
	auto& targetStr = target->ToString();
#endif

	return CBooleanValue::Construct(ctx->m_pRuntime, std::regex_search(targetStr, regex));
}
DEFINE_METHOD(RegexMatch, args)
{
	auto toMatch = args[0];
	auto target = args[1];

	if (toMatch->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.match 1. expected a string, but got \"{}\""), toMatch->TypeAsString()));

	if (target->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.match 2. expected a string, but got \"{}\""), target->TypeAsString()));


#ifdef UNICODE
	std::regex regex(LocaleConverter::u16string_to_ansi(toMatch->ToString()));
	auto targetStr = LocaleConverter::u16string_to_ansi(target->ToString());
#else
	std::regex regex(toMatch->ToString());
	auto& targetStr = target->ToString();
#endif

	return CBooleanValue::Construct(ctx->m_pRuntime, std::regex_match(targetStr, regex));
}
DEFINE_METHOD(RegexReplace, args)
{
	auto toMatch = args[0];
	auto source = args[1];
	auto target = args[2];

	if (toMatch->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.replace 1. expected a string, but got \"{}\""), toMatch->TypeAsString()));

	if (source->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.replace 2. expected a string, but got \"{}\""), source->TypeAsString()));

	if (target->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("regex.replace 3. expected a string, but got \"{}\""), target->TypeAsString()));


#ifdef UNICODE
	std::regex regex(LocaleConverter::u16string_to_ansi(toMatch->ToString()));
	auto sourceStr = LocaleConverter::u16string_to_ansi(source->ToString());
	auto targetStr = LocaleConverter::u16string_to_ansi(target->ToString());
	return CStringValue::Construct(ctx->m_pRuntime, LocaleConverter::ansi_to_u16string(std::regex_replace(sourceStr, regex, targetStr)));

#else
	std::regex regex(toMatch->ToString());
	auto& sourceStr = source->ToString();
	auto& targetStr = target->ToString();
	return CStringValue::Construct(ctx->m_pRuntime, std::regex_replace(sourceStr, regex, targetStr));

#endif


}