#pragma once

#include "varjus_api/types/default.hpp"
#include "internal/aggregate.hpp"

#include <memory>

#include <vector>

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using __KeyValue = std::pair<A, B>;

class CInternalObjectValue;

using ObjectInitializer = VectorOf<__KeyValue<std::size_t, IValue*>>;

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

	[[nodiscard]] constexpr auto& GetAllRuntimeMembers() noexcept { return Get().GetRuntimeInformation(); }


protected:
	CAggregate m_oValue;
};

using __ObjectValues = VectorOf<std::pair<IValue*, IValue*>>;

namespace Varjus {
	struct BuiltInMethod_t;
	struct BuiltInProperty_t;
	class CBuiltInObject;
	class CProgramInformation;
}

class CObjectValue : public CValue<CInternalObjectValue>
{
public:
	CObjectValue() = default;
	~CObjectValue();

	[[nodiscard]] static CObjectValue* Construct(Varjus::CProgramRuntime* const runtime, __ObjectValues&& values);
	[[nodiscard]] static CObjectValue* _ConstructInternal(Varjus::CProgramRuntime* const runtime, ObjectInitializer&& values);

	[[nodiscard]] static std::unique_ptr<Varjus::BuiltInMethod_t> ConstructMethods(Varjus::CProgramInformation* const info);
	[[nodiscard]] static std::unique_ptr<Varjus::BuiltInProperty_t> ConstructProperties(Varjus::CProgramInformation* const info);

	//copy constructor
	[[nodiscard]] Varjus::EValueType Type() const noexcept override { return Varjus::t_object; };

	virtual void Release() override;

	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] virtual const constexpr Varjus::CBuiltInObject* ToBuiltInObject() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr Varjus::CBuiltInObject* ToBuiltInObject() noexcept { return nullptr; }

	[[nodiscard]] virtual IValue* Copy() override;
	[[nodiscard]] CInternalObjectValue* Internal();
	[[nodiscard]] CInternalObjectValue* Internal() const;

	[[nodiscard]] virtual IValue* Index(CRuntimeContext* const ctx, IValue* index) override;
	[[nodiscard]] virtual IValue* GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) override;
	void AddAttribute(IValue* const key, IValue* value);


	[[nodiscard]] CObjectValue* ToObject() override { return this; }
	[[nodiscard]] const CObjectValue* ToObject() const override { return this; }

	[[nodiscard]] std::size_t AddressOf() const noexcept override {
		return reinterpret_cast<std::size_t>(GetShared().get());
	}

protected:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("object"); }
	[[nodiscard]] virtual VarjusString ValueAsString() const override;
	[[nodiscard]] virtual VarjusString ValueAsEscapedString() const override { return ValueAsString(); }

};