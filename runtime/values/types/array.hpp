#pragma once

#include "internal/aggregate.hpp"
#include "internal/method.hpp"

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


class CArrayValue final : public CValue<CInternalArrayValue>, public CDataTypeMethods<CArrayValue>
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
	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return CDataTypeMethods::IsCallable(); }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(std::int64_t index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;

	[[nodiscard]] std::size_t AddressOf() const noexcept override { 
		return reinterpret_cast<std::size_t>(GetShared().get()); 
	}



private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;

	[[nodiscard]] IValue* Push(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* PushFront(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* Pop(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* PopFront(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Map(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* Find(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* FindLast(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Filter(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Contains(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Reverse(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* Join(CRuntimeContext* const ctx, const IValues& newValue);

	//const CBuiltInMethod<CArrayValue>* m_pMethod{ nullptr };
};

