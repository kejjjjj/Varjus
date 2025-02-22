#pragma once

#include "default.hpp"
#include "internal/aggregate.hpp"

#include <vector>
#include <memory>
#include <string>

class CInternalArrayValue;
class CVariable;

template<typename K, typename V>
using KeyValue = std::pair<K, V>;

namespace runtime::__internal {
	VectorOf<ElementIndex>& GetAggregateArrayData();
}

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

	void Set(VectorOf<IValue*>&& v);
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
	
	static CArrayValue* Construct(IValues&& values);
	static void ConstructMethods(); //only called once during init
	static void ConstructProperties(); //only called once during init

	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(IValue* index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] std::size_t AddressOf() const noexcept override { 
		return reinterpret_cast<std::size_t>(GetShared().get()); 
	}

	[[nodiscard]] VectorOf<CVariable*>& GetVariables();
	[[nodiscard]] VectorOf<CVariable*>& GetVariables() const;


private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;
	
	static std::shared_ptr<struct BuiltInMethod_t> m_oMethods;
	static std::shared_ptr<struct BuiltInProperty_t> m_oProperties;
};


