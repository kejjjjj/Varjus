#pragma once
#include "default.hpp"
#include "varjus_api/types/internal/references.hpp"

#include <unordered_map>
#include <vector>

class CRuntimeFunctionBase;
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
	[[nodiscard]] auto& GetCaptures() { return m_oCaptures; }

	void Bind(IValue* target) noexcept { m_pBoundTo = target; }
	[[nodiscard]] auto GetBinding() noexcept { return m_pBoundTo; }

	constexpr void SetCallable(CRuntimeFunctionBase* c) noexcept { m_pCallable = c; }
	[[nodiscard]] auto GetCallable() noexcept { return m_pCallable; }
	[[nodiscard]] auto GetCallable() const noexcept { return m_pCallable; }

	void Release();

	constexpr void SetModuleIndex(std::size_t mIndex) noexcept { m_uModule = mIndex; m_bRequiresModuleChange = true; }
#ifndef RUNNING_TESTS
private:
#endif
	CRuntimeFunctionBase* m_pCallable{ nullptr };
	IValue* m_pBoundTo{ nullptr };
	std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher> m_oCaptures;
	std::size_t m_uModule{};
	bool m_bRequiresModuleChange{ false };

};

class CCallableValue final : public CValue<CInternalCallableValue>
{
public:
	CCallableValue() = default;

	[[nodiscard]] static CCallableValue* Construct(CProgramRuntime* const runtime, CRuntimeFunctionBase* v);

	[[nodiscard]] EValueType Type() const noexcept override { return t_callable; };

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

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
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("callable"); }
	[[nodiscard]] VarjusString ValueAsString() const override { 
		return fmt::format(VSL("{:x}"), (std::size_t)GetShared()->GetCallable()); }

};
