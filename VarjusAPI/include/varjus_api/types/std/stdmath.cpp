#include "stdmath.hpp"

#include "runtime/misc/defs.hpp"

#include "varjus_api/types/types.hpp"
#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/structure.hpp"

#include <random>

using namespace Varjus;

FORWARD_DECLARE_METHOD(Sqrt);
FORWARD_DECLARE_METHOD(Abs);
FORWARD_DECLARE_METHOD(Acos);
FORWARD_DECLARE_METHOD(Atan);
FORWARD_DECLARE_METHOD(Ceil);
FORWARD_DECLARE_METHOD(Floor);
FORWARD_DECLARE_METHOD(Round);
FORWARD_DECLARE_METHOD(Exp);
FORWARD_DECLARE_METHOD(Log);
FORWARD_DECLARE_METHOD(Log10);
FORWARD_DECLARE_METHOD(Sin);
FORWARD_DECLARE_METHOD(Cos);
FORWARD_DECLARE_METHOD(Tan);
FORWARD_DECLARE_METHOD(Asin);
FORWARD_DECLARE_METHOD(Sinh);
FORWARD_DECLARE_METHOD(Cosh);
FORWARD_DECLARE_METHOD(Tanh);
FORWARD_DECLARE_METHOD(Atanh);
FORWARD_DECLARE_METHOD(Acosh);
FORWARD_DECLARE_METHOD(Asinh);
FORWARD_DECLARE_METHOD(Trunc);
FORWARD_DECLARE_METHOD(Sign);

FORWARD_DECLARE_METHOD(Pow);
FORWARD_DECLARE_METHOD(Atan2);
FORWARD_DECLARE_METHOD(Fmod);
FORWARD_DECLARE_METHOD(Hypot);
FORWARD_DECLARE_METHOD(Max);
FORWARD_DECLARE_METHOD(Min);
FORWARD_DECLARE_METHOD(Random);

FORWARD_DECLARE_PROPERTY(Math_PI);
FORWARD_DECLARE_PROPERTY(Math_TAU);
FORWARD_DECLARE_PROPERTY(Math_E);
FORWARD_DECLARE_PROPERTY(Math_SQRT2);
FORWARD_DECLARE_PROPERTY(Math_SQRT3);
FORWARD_DECLARE_PROPERTY(Math_SQRT5);
FORWARD_DECLARE_PROPERTY(Math_GOLDEN);
FORWARD_DECLARE_PROPERTY(Math_LN2);
FORWARD_DECLARE_PROPERTY(Math_LN10);
FORWARD_DECLARE_PROPERTY(Math_LOG2E);
FORWARD_DECLARE_PROPERTY(Math_LOG10E);
FORWARD_DECLARE_PROPERTY(Math_INV_PI);
FORWARD_DECLARE_PROPERTY(Math_INV_SQRT2);
FORWARD_DECLARE_PROPERTY(Math_DEG2RAD);
FORWARD_DECLARE_PROPERTY(Math_RAD2DEG);


#define SINGLE_ARG_METHOD(name, func) receiver.AddMethod(name, func, 1u)
#define TWO_ARG_METHOD(name, func) receiver.AddMethod(name, func, 2u)


void CMathValue::Construct(ObjectDeclaration_t& receiver)
{

	SINGLE_ARG_METHOD(VSL("sqrt"), Sqrt);
	SINGLE_ARG_METHOD(VSL("abs"), Abs);
	SINGLE_ARG_METHOD(VSL("acos"), Acos);
	SINGLE_ARG_METHOD(VSL("atan"), Atan);
	SINGLE_ARG_METHOD(VSL("ceil"), Ceil);
	SINGLE_ARG_METHOD(VSL("floor"), Floor);
	SINGLE_ARG_METHOD(VSL("round"), Round);
	SINGLE_ARG_METHOD(VSL("exp"), Exp);
	SINGLE_ARG_METHOD(VSL("log"), Log);
	SINGLE_ARG_METHOD(VSL("log10"), Log10);
	SINGLE_ARG_METHOD(VSL("sin"), Sin);
	SINGLE_ARG_METHOD(VSL("cos"), Cos);
	SINGLE_ARG_METHOD(VSL("tan"), Tan);
	SINGLE_ARG_METHOD(VSL("asin"), Asin);
	SINGLE_ARG_METHOD(VSL("sinh"), Sinh);
	SINGLE_ARG_METHOD(VSL("cosh"), Cosh);
	SINGLE_ARG_METHOD(VSL("tanh"), Tanh);
	SINGLE_ARG_METHOD(VSL("atanh"), Atanh);
	SINGLE_ARG_METHOD(VSL("acosh"), Acosh);
	SINGLE_ARG_METHOD(VSL("asinh"), Asinh);
	SINGLE_ARG_METHOD(VSL("trunc"), Trunc);
	SINGLE_ARG_METHOD(VSL("sign"), Sign);

	TWO_ARG_METHOD(VSL("pow"), Pow);
	TWO_ARG_METHOD(VSL("atan2"), Atan2);
	TWO_ARG_METHOD(VSL("fmod"), Fmod);
	TWO_ARG_METHOD(VSL("hypot"), Hypot);
	TWO_ARG_METHOD(VSL("max"), Max);
	TWO_ARG_METHOD(VSL("min"), Min);
	TWO_ARG_METHOD(VSL("random"), Random);

	receiver.AddProperty(VSL("pi"), Math_PI);
	receiver.AddProperty(VSL("tau"), Math_TAU);
	receiver.AddProperty(VSL("e"), Math_E);
	receiver.AddProperty(VSL("sqrt2"), Math_SQRT2);
	receiver.AddProperty(VSL("sqrt3"), Math_SQRT3);
	receiver.AddProperty(VSL("sqrt5"), Math_SQRT5);
	receiver.AddProperty(VSL("golden"), Math_GOLDEN);
	receiver.AddProperty(VSL("ln2"), Math_LN2);
	receiver.AddProperty(VSL("ln10"), Math_LN10);
	receiver.AddProperty(VSL("log2e"), Math_LOG2E);
	receiver.AddProperty(VSL("log10e"), Math_LOG10E);
	receiver.AddProperty(VSL("inv_pi"), Math_INV_PI);
	receiver.AddProperty(VSL("inv_sqrt2"), Math_INV_SQRT2);
	receiver.AddProperty(VSL("deg2rad"), Math_DEG2RAD);
	receiver.AddProperty(VSL("rad2deg"), Math_RAD2DEG);
}



#define ConstructDouble(v) CDoubleValue::Construct(ctx->m_pRuntime, v)

DEFINE_PROPERTY(Math_PI) { return ConstructDouble(3.141592653589793); }
DEFINE_PROPERTY(Math_TAU) { return ConstructDouble(6.283185307179586); }
DEFINE_PROPERTY(Math_E) { return ConstructDouble(2.718281828459045); }
DEFINE_PROPERTY(Math_SQRT2) { return ConstructDouble(1.414213562373095); }
DEFINE_PROPERTY(Math_SQRT3) { return ConstructDouble(1.732050807568877); }
DEFINE_PROPERTY(Math_SQRT5) { return ConstructDouble(2.236067977499790); }
DEFINE_PROPERTY(Math_GOLDEN) { return ConstructDouble(1.618033988749895); }
DEFINE_PROPERTY(Math_LN2) { return ConstructDouble(0.693147180559945); }
DEFINE_PROPERTY(Math_LN10) { return ConstructDouble(2.302585092994046); }
DEFINE_PROPERTY(Math_LOG2E) { return ConstructDouble(1.442695040888963); }
DEFINE_PROPERTY(Math_LOG10E) { return ConstructDouble(0.434294481903252); }
DEFINE_PROPERTY(Math_INV_PI) { return ConstructDouble(0.318309886183790); }
DEFINE_PROPERTY(Math_INV_SQRT2) { return ConstructDouble(0.707106781186548); }
DEFINE_PROPERTY(Math_DEG2RAD) { return ConstructDouble(0.017453292519943); }
DEFINE_PROPERTY(Math_RAD2DEG) { return ConstructDouble(57.29577951308232); }

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(name, func) \
DEFINE_METHOD(name, args){\
	const auto& v = args.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL(#name), v->TypeAsString())); \
	return CDoubleValue::Construct(ctx->m_pRuntime, func(v->ToDouble())); }\

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(name, func, type, T) \
DEFINE_METHOD(name, args){\
	const auto& v = args.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL(#name), v->TypeAsString())); \
	return type::Construct(ctx->m_pRuntime, static_cast<T>(func(v->ToDouble()))); }\

#define DEFINE_TWO_ARG_GENERIC_MATH_FUNC(name, func) \
DEFINE_METHOD(name, args){\
	const auto& lhs = args[0]; \
	const auto& rhs = args[1]; \
	if (!lhs->IsArithmetic()) \
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL(#name), lhs->TypeAsString())); \
	if (!rhs->IsArithmetic()) \
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL(#name), rhs->TypeAsString())); \
	return CDoubleValue::Construct(ctx->m_pRuntime, func(lhs->ToDouble(), rhs->ToDouble())); }\

DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Sqrt, std::sqrt);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Abs, std::abs);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Acos, std::acos);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Atan, std::atan);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Ceil, std::ceil);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Floor, std::floor);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Round, std::round);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Exp, std::exp);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Log, std::log);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Log10, std::log10);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Sin, std::sin);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Cos, std::cos);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Tan, std::tan);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Asin, std::asin);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Sinh, std::sinh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Cosh, std::cosh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Tanh, std::tanh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Atanh, std::atanh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Acosh, std::acosh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(Asinh, std::asinh);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(Trunc, std::trunc, CIntValue, VarjusInt);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(Sign, std::signbit, CBooleanValue, bool);

DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Pow, std::pow);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Atan2, std::atan2);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Fmod, std::fmod);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Hypot, std::hypot);

DEFINE_METHOD(Max, args){
	const auto& lhs = args[0]; 
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic()) 
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("max"), lhs->TypeAsString()));
	if (!rhs->IsArithmetic()) 
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("max"), rhs->TypeAsString()));
	return lhs->ToDouble() < rhs->ToDouble() ? rhs->Copy() : lhs->Copy();
}
DEFINE_METHOD(Min, args) {

	const auto& lhs = args[0];
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic())
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("min"), lhs->TypeAsString()));
	if (!rhs->IsArithmetic())
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("min"), rhs->TypeAsString()));
	return rhs->ToDouble() > lhs->ToDouble() ? lhs->Copy() : rhs->Copy();
}

inline VarjusDouble GetRandomDouble(VarjusDouble min, VarjusDouble max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return dis(gen);
}


DEFINE_METHOD(Random, args) {

	const auto& lhs = args[0];
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic())
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("random"), lhs->TypeAsString()));
	if (!rhs->IsArithmetic())
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} expected an arithmetic value, but got \"{}\""), VSL("random"), rhs->TypeAsString()));
	
	const auto a = lhs->ToDouble();
	const auto b = rhs->ToDouble();

	if(b <= a)
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("math.{} min >= max"), VSL("random")));

	return CDoubleValue::Construct(ctx->m_pRuntime, GetRandomDouble(a, b));
}
