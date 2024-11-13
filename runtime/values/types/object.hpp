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

class CObjectValue final : public CValue<std::shared_ptr<CInternalObjectValue>>
{
public:
	CObjectValue() = default;
	CObjectValue(ObjectInitializer&& v);
	~CObjectValue();

	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_object; };

	void CreateOwnership();
	void Release() override;

	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }

	[[nodiscard]] CObjectValue* MakeShared() const;
	[[nodiscard]] CObjectValue* Copy() override;
	[[nodiscard]] CInternalObjectValue* Internal();
	[[nodiscard]] CInternalObjectValue* Internal() const;

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "object"s; }
	[[nodiscard]] std::string ValueAsString() const override;

};

class CInternalObjectValue final : public CValue<CAggregate>
{
public:
	CInternalObjectValue() = default;
	CInternalObjectValue(ObjectInitializer&& v);
	~CInternalObjectValue();

	void Release() override;

	void Set(ObjectInitializer&& v);

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return m_oValue; }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return m_oValue; }
};
