#include "math.hpp"

#include "api/types/operators/default_operators.hpp"

#include "linter/context.hpp"

#include "runtime/exceptions/exception.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"

#include <random>

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


#define SINGLE_ARG_METHOD(name, func) m_oMethods.AddMethod(name, func, 1u)
#define TWO_ARG_METHOD(name, func) m_oMethods.AddMethod(name, func, 2u)


BuiltInMethod_t CMathValue::ConstructMethods()
{
	BuiltInMethod_t m_oMethods;

	SINGLE_ARG_METHOD("sqrt", Sqrt);
	SINGLE_ARG_METHOD("abs", Abs);
	SINGLE_ARG_METHOD("acos", Acos);
	SINGLE_ARG_METHOD("atan", Atan);
	SINGLE_ARG_METHOD("ceil", Ceil);
	SINGLE_ARG_METHOD("floor", Floor);
	SINGLE_ARG_METHOD("round", Round);
	SINGLE_ARG_METHOD("exp", Exp);
	SINGLE_ARG_METHOD("log", Log);
	SINGLE_ARG_METHOD("log10", Log10);
	SINGLE_ARG_METHOD("sin", Sin);
	SINGLE_ARG_METHOD("cos", Cos);
	SINGLE_ARG_METHOD("tan", Tan);
	SINGLE_ARG_METHOD("asin", Asin);
	SINGLE_ARG_METHOD("sinh", Sinh);
	SINGLE_ARG_METHOD("cosh", Cosh);
	SINGLE_ARG_METHOD("tanh", Tanh);
	SINGLE_ARG_METHOD("atanh", Atanh);
	SINGLE_ARG_METHOD("acosh", Acosh);
	SINGLE_ARG_METHOD("asinh", Asinh);
	SINGLE_ARG_METHOD("trunc", Trunc);
	SINGLE_ARG_METHOD("sign", Sign);

	TWO_ARG_METHOD("pow", Pow);
	TWO_ARG_METHOD("atan2", Atan2);
	TWO_ARG_METHOD("fmod", Fmod);
	TWO_ARG_METHOD("hypot", Hypot);
	TWO_ARG_METHOD("max", Max);
	TWO_ARG_METHOD("min", Min);

	TWO_ARG_METHOD("random", Random);


	return m_oMethods;
}

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(name, func) \
DEFINE_METHOD(name){\
	const auto& v = args.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, v->TypeAsString())); \
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(func(v->ToDouble())); }\

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(name, func, type, T) \
DEFINE_METHOD(name){\
	const auto& v = args.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, v->TypeAsString())); \
	return CProgramRuntime::AcquireNewValue<type>(static_cast<T>(func(v->ToDouble()))); }\

#define DEFINE_TWO_ARG_GENERIC_MATH_FUNC(name, func) \
DEFINE_METHOD(name){\
	const auto& lhs = args[0]; \
	const auto& rhs = args[1]; \
	if (!lhs->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, lhs->TypeAsString())); \
	if (!rhs->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, rhs->TypeAsString())); \
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(func(lhs->ToDouble(), rhs->ToDouble())); }\

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
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(Trunc, std::trunc, CIntValue, std::int64_t);
DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(Sign, std::signbit, CIntValue, std::int64_t);

DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Pow, std::pow);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Atan2, std::atan2);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Fmod, std::fmod);
DEFINE_TWO_ARG_GENERIC_MATH_FUNC(Hypot, std::hypot);

DEFINE_METHOD(Max){
	const auto& lhs = args[0]; 
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic()) 
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "max", lhs->TypeAsString()));
	if (!rhs->IsArithmetic()) 
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "max", rhs->TypeAsString()));
	return lhs->ToDouble() < rhs->ToDouble() ? rhs->Copy() : lhs->Copy();
}
DEFINE_METHOD(Min) {

	const auto& lhs = args[0];
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "min", lhs->TypeAsString()));
	if (!rhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "min", rhs->TypeAsString()));
	return rhs->ToDouble() > lhs->ToDouble() ? lhs->Copy() : rhs->Copy();
}

inline double GetRandomDouble(double min, double max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return dis(gen);
}


DEFINE_METHOD(Random) {

	const auto& lhs = args[0];
	const auto& rhs = args[1];
	if (!lhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "random", lhs->TypeAsString()));
	if (!rhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "random", rhs->TypeAsString()));
	
	const auto a = lhs->ToDouble();
	const auto b = rhs->ToDouble();

	if(b <= a)
		throw CRuntimeError(std::format("math.{} min >= max", "random"));

	return CProgramRuntime::AcquireNewValue<CDoubleValue>(GetRandomDouble(a, b));
}
