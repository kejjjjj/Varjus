#pragma once

#include "internal/aggregate.hpp"
#include "runtime/values/types/simple.hpp"

#include <vector>
#include <memory>
#include <string>

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

class CInternalObjectValue;

using ObjectInitializer = VectorOf<KeyValue<ElementIndex, IValue*>>;

class CInternalObjectValue final
{
public:
	CInternalObjectValue() = default;
	~CInternalObjectValue();

	void Release();

	void Set(ObjectInitializer&& v);
	constexpr auto& Get() noexcept { return m_oValue; }
	constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return Get(); }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return Get(); }

protected:
	CAggregate m_oValue;
};

class CObjectValue final : public CValue<CInternalObjectValue>
{
public:
	CObjectValue() = default;
	~CObjectValue();

	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_object; };

	void Release() override;

	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }

	[[nodiscard]] CObjectValue* Copy() override;
	[[nodiscard]] CInternalObjectValue* Internal();
	[[nodiscard]] CInternalObjectValue* Internal() const;

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] CObjectValue* ToObject() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "object"s; }
	[[nodiscard]] std::string ValueAsString() const override;

};

