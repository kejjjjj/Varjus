#pragma once
#include "simple.hpp"
#include <format>
#include <unordered_map>
#include <vector>

class CRuntimeFunction;
class CFunction;
class CVariable;

using ElementIndex = std::size_t;

struct CBuiltInMemberFunctionContent
{
	void Release();

	IValue* m_pThis{ nullptr };
	const struct CArrayBuiltInMethod* m_pMethod{ nullptr };
};

class CBuiltInMemberCallableValue : public CValue<CBuiltInMemberFunctionContent>
{
public:
	CBuiltInMemberCallableValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_builtin_member_callable; };

	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return Get().m_pThis->Type() != t_undefined; }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBuiltInMemberCallable() const noexcept override { return true; }

	[[nodiscard]] CBuiltInMemberCallableValue* ToBuiltInMemberCallable() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "method"s; }
	[[nodiscard]] std::string ValueAsString() const override {
		return std::format("{:x}", (std::size_t)Get().m_pThis);
	}
};
