#pragma once
#include "simple.hpp"

class CStringValue : public CValue<std::string>
{
public:
	CStringValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_string; };

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }

	[[nodiscard]] bool ToBoolean() const override { return !Get().empty(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }
	[[nodiscard]] const std::string& ToString() const override { return Get(); }

	[[nodiscard]] IValue* Index(std::int64_t index) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "string"s; }
	[[nodiscard]] std::string ValueAsString() const override { return '\"' + Get() + '\"'; }

public:
};
