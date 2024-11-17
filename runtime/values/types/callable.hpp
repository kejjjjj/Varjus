#pragma once
#include "simple.hpp"
#include <format>
#include <unordered_map>
#include <vector>

class CRuntimeFunction;
class CFunction;
class CVariable;

template<typename T>
using VectorOf = std::vector<T>;

using VariableIndex = std::size_t;

class CCallableValue : public CValue<CRuntimeFunction*>
{
public:
	CCallableValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_callable; };

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return !!Get(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return true; }
	[[nodiscard]] CCallableValue* ToCallable() override { return this; }

	void SetCaptures(CFunction* const thisFunction, const VectorOf<VariableIndex>& captures);
	auto& GetCaptures() { return m_oCaptures; }
private:
	[[nodiscard]] std::string TypeAsString() const override { return "callable"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::format("{:x}", (std::size_t)Get()); }

	std::unordered_map<VariableIndex, CVariable*> m_oCaptures;
public:
};
