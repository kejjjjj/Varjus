#pragma once
#include "simple.hpp"
#include "linter/modules/references.hpp"

#include <unordered_map>
#include <vector>
#include <format>


class CRuntimeFunction;
class CFunction;
class CVariable;

template<typename T>
using VectorOf = std::vector<T>;

using VariableIndex = std::size_t;

class CInternalCallableValue final
{
	friend class CCallableValue;

public:
	CInternalCallableValue() = default;
	~CInternalCallableValue() = default;

	void SetCaptures(CRuntimeContext* const ctx, const VectorOf<CCrossModuleReference>& captures);
	auto& GetCaptures() { return m_oCaptures; }

	auto& GetCallable() noexcept { return m_pCallable; }
	auto& GetCallable() const noexcept { return m_pCallable; }

	void Release();

	constexpr void SetModuleIndex(std::size_t mIndex) noexcept { m_uModule = mIndex; m_bRequiresModuleChange = true; }

//#ifndef RUNNING_TESTS
//private:
//#endif

	CRuntimeFunction* m_pCallable{ nullptr };
	std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher> m_oCaptures;
	std::size_t m_uModule{};
	bool m_bRequiresModuleChange{ false };
};

class CCallableValue final : public CValue<CInternalCallableValue>
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
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }

	[[nodiscard]] CCallableValue* ToCallable() override { return this; }

	[[nodiscard]] CInternalCallableValue* Internal();
	[[nodiscard]] CInternalCallableValue* Internal() const;

	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;
		
	[[nodiscard]] std::size_t AddressOf() const noexcept override {
		return reinterpret_cast<std::size_t>(GetShared().get());
	}

private:
	[[nodiscard]] std::string TypeAsString() const override { return "callable"s; }
	[[nodiscard]] std::string ValueAsString() const override { 
		return std::format("{:x}", (std::size_t)GetShared()->GetCallable()); }

};
