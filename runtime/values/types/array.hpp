#pragma once

#include "internal/aggregate.hpp"
#include "runtime/values/types/simple.hpp"



#include <vector>
#include <memory>
#include <string>

class CInternalArrayValue;

template<typename K, typename V>
using KeyValue = std::pair<K, V>;

namespace runtime::__internal {
	VectorOf<ElementIndex>& GetAggregateArrayData();

	VectorOf<KeyValue<ElementIndex, IValue*>> GetBuiltinMethods();

}

struct CArrayContent final
{
	CArrayAggregate m_oAggregate;
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

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return Get().m_oAggregate; }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return Get().m_oAggregate; }

	[[nodiscard]] std::size_t Length() const noexcept;

protected:
	CArrayContent m_oValue;
};


class CArrayValue final : public CValue<CInternalArrayValue>
{
	friend struct CStaticArrayBuiltInMethods;
public:
	CArrayValue() = default;
	~CArrayValue();
	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return !!m_pMethod; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(std::int64_t index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] IValue* Call(CFunction* const thisFunction, const IValues& args) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;

	[[nodiscard]] IValue* Push(const IValues& newValue);
	
	const struct CArrayBuiltInMethod* m_pMethod{ nullptr };
};

