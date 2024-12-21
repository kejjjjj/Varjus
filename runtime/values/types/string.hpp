#pragma once
#include "simple.hpp"
#include "internal/aggregate.hpp"
#include "internal/method.hpp"


struct CStringContent final
{
	CArrayAggregate m_oAggregate;
	std::string m_sString;
};

class CInternalStringValue final
{
public:
	CInternalStringValue() = default;
	~CInternalStringValue();

	void Release();

	void Set(const std::string& value);
	constexpr auto& Get() noexcept { return m_oValue; }
	constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetString() noexcept { return Get().m_sString; }
	[[nodiscard]] constexpr auto& GetString() const noexcept { return Get().m_sString; }

	[[nodiscard]] constexpr auto& GetAggregateValue() const noexcept { return Get().m_oAggregate; }
	[[nodiscard]] constexpr auto& GetAggregateValue() noexcept { return Get().m_oAggregate; }

	[[nodiscard]] std::size_t Length() const noexcept;

protected:
	CStringContent m_oValue;
};

class CStringValue final : public CValue<CInternalStringValue>, public CDataTypeMethods<CStringValue>
{
	using StringMethods = CBuiltInMethods<CStringValue>::InputType;

public:
	CStringValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_string; };

	[[nodiscard]] IValue* Copy() override;

	static CStringValue* Construct(const std::string& v);
	static StringMethods ConstructMethods();

	virtual void Release() override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return CDataTypeMethods::IsCallable(); }

	[[nodiscard]] bool ToBoolean() const override { return !Internal()->GetString().empty(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }
	[[nodiscard]] const std::string& ToString() const override { return Internal()->GetString(); }

	[[nodiscard]] CStringValue* ToCString() override { return this; }

	[[nodiscard]] CInternalStringValue* Internal();
	[[nodiscard]] const CInternalStringValue* Internal() const;

	[[nodiscard]] IValue* Index(std::int64_t index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;


private:
	[[nodiscard]] std::string TypeAsString() const override { return "string"s; }
	[[nodiscard]] std::string ValueAsString() const override { return Internal()->GetString(); }


	[[nodiscard]] IValue* ToUpper(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* ToLower(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Substring(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* Split(CRuntimeContext* const ctx, const IValues& newValue);

	[[nodiscard]] IValue* Replace(CRuntimeContext* const ctx, const IValues& newValue);
	[[nodiscard]] IValue* Repeat(CRuntimeContext* const ctx, const IValues& newValue);
	
	[[nodiscard]] IValue* GetCodeAt(CRuntimeContext* const ctx, const IValues& newValue);

};
