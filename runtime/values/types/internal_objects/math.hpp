#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/method.hpp"




class CMathValue final : public CObjectValue, public CDataTypeMethods<CMathValue>
{
	using MathMethods = CBuiltInMethods<CMathValue>::InputType;

public:

	CMathValue() : CObjectValue() {}

	static CMathValue* Construct();
	static MathMethods ConstructMethods();

	void Release() override;
	IValue* Copy() override;

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return CDataTypeMethods::IsCallable(); }

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;
	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;

private:
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
};
