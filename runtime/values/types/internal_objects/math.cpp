#include "math.hpp"
#include "runtime/runtime.hpp"
#include "runtime/values/simple_operators.hpp"

#define SINGLE_ARG_METHOD(name, func) {name,   {1u, &CMathValue::func}}
#define TWO_ARG_METHOD(name, func) {name,   {2u, &CMathValue::func}}

CMathValue::MathMethods CMathValue::ConstructMethods()
{
	return {
		SINGLE_ARG_METHOD("sqrt", Sqrt),
		SINGLE_ARG_METHOD("abs", Abs),
		SINGLE_ARG_METHOD("acos", Acos),
		SINGLE_ARG_METHOD("atan", Atan),
		SINGLE_ARG_METHOD("ceil", Ceil),
		SINGLE_ARG_METHOD("floor", Floor),
		SINGLE_ARG_METHOD("round", Round),
		SINGLE_ARG_METHOD("exp", Exp),
		SINGLE_ARG_METHOD("log", Log),
		SINGLE_ARG_METHOD("log10", Log10),
		SINGLE_ARG_METHOD("sin", Sin),
		SINGLE_ARG_METHOD("cos", Cos),
		SINGLE_ARG_METHOD("tan", Tan),
		SINGLE_ARG_METHOD("asin", Asin),
		SINGLE_ARG_METHOD("sinh", Sinh),
		SINGLE_ARG_METHOD("cosh", Cosh),
		SINGLE_ARG_METHOD("tanh", Tanh),
		SINGLE_ARG_METHOD("atanh", Atanh),
		SINGLE_ARG_METHOD("acosh", Acosh),
		SINGLE_ARG_METHOD("asinh", Asinh),
		SINGLE_ARG_METHOD("trunc", Trunc),
		SINGLE_ARG_METHOD("sign", Sign),

		TWO_ARG_METHOD("pow", Pow),
		TWO_ARG_METHOD("atan2", Atan2),
		TWO_ARG_METHOD("fmod", Fmod),
		TWO_ARG_METHOD("hypot", Hypot),
		TWO_ARG_METHOD("max", Max),
		TWO_ARG_METHOD("min", Min),

	};
};

CMathValue* CMathValue::Construct()
{
	auto ptr = CProgramRuntime::AcquireNewValue<CMathValue>();
	ptr->MakeShared();
	return ptr;
}

void CMathValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CDataTypeMethods::Release();
	CProgramRuntime::FreeValue<CMathValue>(this);
	ReleaseShared();
}

IValue* CMathValue::Copy() {

	if (auto c = CDataTypeMethods::Copy())
		return c;

	CMathValue* ptr = CProgramRuntime::AcquireNewValue<CMathValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();

	return ptr;
}

IValue* CMathValue::GetAggregate(std::size_t memberIdx) {

	if (auto func = CDataTypeMethods::FindMethod(memberIdx)) {
		auto newValue = HasOwner() ? this : static_cast<CMathValue*>(this->Copy());
		newValue->SetMethod(func);
		return newValue;
	}

	return CObjectValue::GetAggregate(memberIdx);
}

IValue* CMathValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	if(!IsCallable())
		throw CRuntimeError(std::format("a value of type \"{}\" is not callable", TypeAsString()));

	auto ret = CBuiltInMethods<CMathValue>::CallMethod(ctx, this, args, GetMethod());
	CDataTypeMethods::Release();
	return ret;
}

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC(name, func) \
IValue* CMathValue::name([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values) {\
	const auto& v = values.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, v->TypeAsString())); \
	return CProgramRuntime::AcquireNewValue<CDoubleValue>(func(v->ToDouble())); }\

#define DEFINE_SINGLE_ARG_GENERIC_MATH_FUNC_TYPE(name, func, type, T) \
IValue* CMathValue::name([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values) {\
	const auto& v = values.front(); \
	if (!v->IsArithmetic()) \
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", #name, v->TypeAsString())); \
	return CProgramRuntime::AcquireNewValue<type>(static_cast<T>(func(v->ToDouble()))); }\

#define DEFINE_TWO_ARG_GENERIC_MATH_FUNC(name, func) \
IValue* CMathValue::name([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values) {\
	const auto& lhs = values[0]; \
	const auto& rhs = values[1]; \
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

IValue* CMathValue::Max([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values)
{
	const auto& lhs = values[0]; 
	const auto& rhs = values[1]; 
	if (!lhs->IsArithmetic()) 
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "max", lhs->TypeAsString()));
	if (!rhs->IsArithmetic()) 
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "max", rhs->TypeAsString()));
	return lhs->ToDouble() < rhs->ToDouble() ? rhs->Copy() : lhs->Copy();
}
IValue* CMathValue::Min([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values)
{
	const auto& lhs = values[0];
	const auto& rhs = values[1];
	if (!lhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "min", lhs->TypeAsString()));
	if (!rhs->IsArithmetic())
		throw CRuntimeError(std::format("math.{} expected an arithmetic value, but got \"{}\"", "min", rhs->TypeAsString()));
	return rhs->ToDouble() > lhs->ToDouble() ? lhs->Copy() : rhs->Copy();
}
