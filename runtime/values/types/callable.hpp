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


class CInternalCallableValue
{
public:
	CInternalCallableValue() = default;
	~CInternalCallableValue() = default;

	void SetCaptures(CFunction* const thisFunction, const VectorOf<VariableIndex>& captures);
	auto& GetCaptures() { return m_oCaptures; }

	auto& GetCallable() noexcept { return m_pCallable; }
	auto& GetCallable() const noexcept { return m_pCallable; }

	void Release();

private:
	
	CRuntimeFunction* m_pCallable{ nullptr };
	std::unordered_map<VariableIndex, CVariable*> m_oCaptures;
};

class CCallableValue : public CValue<CInternalCallableValue>
{
public:
	CCallableValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_callable; };

	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return !!GetShared()->GetCallable(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return true; }
	[[nodiscard]] CCallableValue* ToCallable() override { return this; }

	[[nodiscard]] CInternalCallableValue* Internal();
	[[nodiscard]] CInternalCallableValue* Internal() const;

	[[nodiscard]] IValue* Call(CFunction* const thisFunction, const IValues& args) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "callable"s; }
	[[nodiscard]] std::string ValueAsString() const override { 
		return std::format("{:x}", (std::size_t)GetShared()->GetCallable()); }

};
