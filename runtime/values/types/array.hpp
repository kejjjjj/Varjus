#pragma once

#include "internal/aggregate.hpp"
#include "runtime/values/types/simple.hpp"
#include "runtime/values/types/internal/builtin_methods.hpp"



#include <vector>
#include <memory>
#include <string>

class CInternalArrayValue;

template<typename K, typename V>
using KeyValue = std::pair<K, V>;

namespace runtime::__internal {
	VectorOf<ElementIndex>& GetAggregateArrayData();
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
	using ArrayMethods = CBuiltInMethods<CArrayValue>::InputType;
public:
	CArrayValue() = default;
	~CArrayValue();
	
	static CArrayValue* Construct(IValues&& values);
	static ArrayMethods ConstructMethods();

	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return !!m_pMethod; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(std::int64_t index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] IValue* Call(CFunction* const thisFunction, const IValues& args) override;

	[[nodiscard]] std::size_t AddressOf() const noexcept override { 
		return reinterpret_cast<std::size_t>(GetShared().get()); 
	}



private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;

	[[nodiscard]] IValue* Push(CFunction* const thisFunction, const IValues& newValue);
	[[nodiscard]] IValue* PushFront(CFunction* const thisFunction, const IValues& newValue);
	[[nodiscard]] IValue* Pop(CFunction* const thisFunction, const IValues& newValue);
	[[nodiscard]] IValue* PopFront(CFunction* const thisFunction, const IValues& newValue);

	[[nodiscard]] IValue* Map(CFunction* const thisFunction, const IValues& newValue);
	[[nodiscard]] IValue* Find(CFunction* const thisFunction, const IValues& newValue);
	[[nodiscard]] IValue* Filter(CFunction* const thisFunction, const IValues& newValue);

	[[nodiscard]] IValue* Contains(CFunction* const thisFunction, const IValues& newValue);

	const CBuiltInMethod<CArrayValue>* m_pMethod{ nullptr };
};

