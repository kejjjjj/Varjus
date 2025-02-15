#pragma once

#include "api/types/default.hpp"
#include "internal/aggregate.hpp"

#include <memory>
#include <string>
#include <vector>

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

class CInternalObjectValue;

using ObjectInitializer = VectorOf<KeyValue<std::size_t, IValue*>>;

class CInternalObjectValue final
{
public:
	CInternalObjectValue() = default;
	~CInternalObjectValue();

	void Release();

	void Set(std::size_t moduleIndex, ObjectInitializer&& v);
	constexpr auto& Get() noexcept { return m_oValue; }
	constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return Get(); }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return Get(); }

protected:
	CAggregate m_oValue;
};

class CObjectValue : public CValue<CInternalObjectValue>
{
public:
	CObjectValue() = default;
	~CObjectValue();

	static CObjectValue* Construct(std::size_t moduleIndex, ObjectInitializer&& values);

	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_object; };

	virtual void Release() override;

	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }

	[[nodiscard]] virtual IValue* Copy() override;
	[[nodiscard]] CInternalObjectValue* Internal();
	[[nodiscard]] CInternalObjectValue* Internal() const;

	[[nodiscard]] IValue* Index(IValue* index) override;
	[[nodiscard]] virtual IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] CObjectValue* ToObject() override { return this; }

	[[nodiscard]] std::size_t AddressOf() const noexcept override {
		return reinterpret_cast<std::size_t>(GetShared().get());
	}

protected:
	[[nodiscard]] std::string TypeAsString() const override { return "object"s; }
	[[nodiscard]] std::string ValueAsString() const override;

};

