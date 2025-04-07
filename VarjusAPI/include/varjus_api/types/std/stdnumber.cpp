#include "stdnumber.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"

#include <optional>
#include <iomanip>
#include <sstream>
#include <cmath>

FORWARD_DECLARE_METHOD(Stoi);
FORWARD_DECLARE_METHOD(Stou);
FORWARD_DECLARE_METHOD(Stod);
FORWARD_DECLARE_METHOD(SetPrecision);

void CStdNumberValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("stoi"),         Stoi,         2);
	receiver.AddMethod(VSL("stou"),         Stou,         2);
	receiver.AddMethod(VSL("stod"),         Stod,         1);
	receiver.AddMethod(VSL("setprecision"), SetPrecision, 2);

}

#ifdef _WIN32
	#ifdef _WIN64
		#define X64 1
	#else
		#define X64 0
	#endif
#else
	#if __x86_64__ || __ppc64__
		#define X64 1
	#else
		#define X64 0
	#endif
#endif

DEFINE_METHOD(Stoi, args)
{
	const auto v = args[0];
	const auto _base = args[1];

	if (v->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.stoi expected a string, but got \"{}\""), v->TypeAsString()));
	}
	if (!_base->IsIntegral()) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.stoi 2. expected an integral type, but got \"{}\""), _base->TypeAsString()));
	}

#ifdef UNICODE
	const auto str = LocaleConverter::u16string_to_ansi(v->ToString());
#else
	const auto& str = v->ToString();
#endif

	try {
#if X64
		return CIntValue::Construct(ctx->m_pRuntime, std::stoll(str, nullptr, static_cast<std::int32_t>(_base->ToInt())));
#else
		return CIntValue::Construct(ctx->m_pRuntime, std::stoi(str, nullptr, _base->ToInt()));
#endif
	} catch (...) {
		//expensive!
		return IValue::Construct(ctx->m_pRuntime);
	}

}
DEFINE_METHOD(Stou, args)
{
	const auto v = args[0];
	const auto _base = args[1];

	if (v->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.stou expected a string, but got \"{}\""), v->TypeAsString()));
	}
	if (!_base->IsIntegral()) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.stou 2. expected an integral type, but got \"{}\""), _base->TypeAsString()));
	}

#ifdef UNICODE
	const auto str = LocaleConverter::u16string_to_ansi(v->ToString());
#else
	const auto& str = v->ToString();
#endif

	try {
#if X64
		return CUIntValue::Construct(ctx->m_pRuntime, std::stoull(str, nullptr, static_cast<std::int32_t>(_base->ToInt())));
#else
		auto r = std::stoul(str, nullptr, _base->ToInt());

		if (r > std::numeric_limits<unsigned int>::max()) {
			throw CRuntimeError(ctx->m_pRuntime, VSL("number.stou Value exceeds unsigned int range"));
		}

		//surely the user keeps the value within unsigned int limits
		return CUIntValue::Construct(ctx->m_pRuntime, r);
#endif
	}
	catch (...) {
		//expensive!
		return IValue::Construct(ctx->m_pRuntime);
	}

}
DEFINE_METHOD(Stod, args)
{
	const auto v = args[0];

	if (v->Type() != t_string) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.stod expected a string, but got \"{}\""), v->TypeAsString()));
	}

#ifdef UNICODE
	const auto str = LocaleConverter::u16string_to_ansi(v->ToString());
#else
	const auto& str = v->ToString();
#endif

	try {
		return CDoubleValue::Construct(ctx->m_pRuntime, std::stod(str));
	}
	catch (...) {
		//expensive!
		return IValue::Construct(ctx->m_pRuntime);
	}

}


inline std::optional<VarjusString> _SetPrecision(double value, VarjusInt decimals) {

	if (decimals < 0)
		return std::nullopt;

	// Handle rounding correctly
	double multiplier = std::pow(10.0, decimals);
	value = std::round(value * multiplier) / multiplier;

	// Convert to string with fixed-point notation
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(decimals) << value;
#ifdef UNICODE
	return std::make_optional(LocaleConverter::ansi_to_u16string(oss.str()));
#else
	return std::make_optional(oss.str());
#endif
}

DEFINE_METHOD(SetPrecision, args)
{
	const auto v = args[0];
	const auto precision = args[1];

	if (v->Type() != t_double) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.setprecision expected a double, but got \"{}\""), v->TypeAsString()));
	}

	if (!precision->IsIntegral()) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("number.setprecision expected an integral type, but got \"{}\""), v->TypeAsString()));
	}

	if (const auto str = _SetPrecision(v->ToDouble(), precision->ToInt()))
		return CStringValue::Construct(ctx->m_pRuntime, *str);

	throw CRuntimeError(ctx->m_pRuntime, VSL("number.setprecision < 0 decimals"));
}