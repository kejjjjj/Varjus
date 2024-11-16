#pragma once
#include "simple.hpp"
#include <format>

class CRuntimeFunction;

class CCallableValue : public CValue<CRuntimeFunction*>
{
public:
	CCallableValue() = default;
	CCallableValue(CRuntimeFunction* callable) : CValue(callable) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_callable; };

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return !!Get(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "callable"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::format("{:x}", (std::size_t)Get()); }

public:
};
