#pragma once

#include "default.hpp"
#include "internal/aggregate.hpp"

#include <vector>
#include <memory>


class CInternalArrayValue;
class CVariable;

template<typename K, typename V>
using KeyValue = std::pair<K, V>;


struct CArrayContent final
{
	VectorOf<CVariable*> m_oVariables;
};

class CInternalArrayValue final
{
public:
	CInternalArrayValue() = default;
	~CInternalArrayValue();

	void Release();

	void Set(CProgramRuntime* const runtime, VectorOf<IValue*>&& v);
	constexpr auto& Get() noexcept { return m_oValue; }
	constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetVariables() noexcept { return Get().m_oVariables; }
	[[nodiscard]] constexpr auto& GetVariables() const noexcept { return Get().m_oVariables; }

	[[nodiscard]] std::size_t Length() const noexcept;

protected:
	CArrayContent m_oValue;
};


class CArrayValue final : public CValue<CInternalArrayValue>
{
public:
	CArrayValue() = default;
	~CArrayValue();
	
	static CArrayValue* Construct(CProgramRuntime* const runtime, IValues&& values);
	[[nodiscard]] static std::unique_ptr<struct BuiltInMethod_t> ConstructMethods(class CProgramInformation* const info);
	[[nodiscard]] static std::unique_ptr<struct BuiltInProperty_t> ConstructProperties(class CProgramInformation* const info);

	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIterable() const noexcept override { return true; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(IValue* index) override;
	[[nodiscard]] IValue* GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) override;
	[[nodiscard]] IValues ToIterable() const override;

	[[nodiscard]] std::size_t AddressOf() const noexcept override { 
		return reinterpret_cast<std::size_t>(GetShared().get()); 
	}

	[[nodiscard]] VectorOf<CVariable*>& GetVariables();
	[[nodiscard]] VectorOf<CVariable*>& GetVariables() const;


private:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("array"); }
	[[nodiscard]] VarjusString ValueAsString() const override;

};


