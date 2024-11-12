#pragma once

#include "internal/aggregate.hpp"
#include "runtime/values/types/simple.hpp"



#include <vector>
#include <memory>
#include <string>

template<typename T>
using VectorOf = std::vector<T>;

class CInternalArrayValue;

namespace runtime::__internal {
	VectorOf<ElementIndex>& GetAggregateArrayData();
}

class CArrayValue final : public CValue<std::shared_ptr<CInternalArrayValue>>
{
public:
	CArrayValue() = default;
	CArrayValue(VectorOf<IValue*>&& v);
	~CArrayValue();
	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void CreateOwnership();
	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }

	[[nodiscard]] CArrayValue* MakeShared() const;
	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(std::int64_t index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;

};

using UniqueVariable = std::unique_ptr<CVariable>;

struct CArrayContent final
{
	CAggregate m_oAggregate;
	VectorOf<UniqueVariable> m_oVariables;
};

class CInternalArrayValue final : public CValue<CArrayContent>
{
public:
	CInternalArrayValue() = default;
	CInternalArrayValue(VectorOf<IValue*>&& v);
	~CInternalArrayValue();

	void Release() override;

	void Set(VectorOf<IValue*>&& v);

	[[nodiscard]] constexpr auto& GetVariables() noexcept { return m_oValue.m_oVariables; }
	[[nodiscard]] constexpr auto& GetVariables() const noexcept { return m_oValue.m_oVariables; }

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return m_oValue.m_oAggregate; }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return m_oValue.m_oAggregate; }

	[[nodiscard]] std::size_t Length() const noexcept;
};
