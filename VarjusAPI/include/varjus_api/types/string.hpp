#pragma once

#include "default.hpp"
#include "internal/aggregate.hpp"


struct CStringContent final
{
	std::string m_sString;
};

class CInternalStringValue final
{
public:
	CInternalStringValue() = default;
	~CInternalStringValue();

	void Release();

	void Set(const std::string& value);
	[[nodiscard]] constexpr auto& Get() noexcept { return m_oValue; }
	[[nodiscard]] constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetString() noexcept { return Get().m_sString; }
	[[nodiscard]] constexpr auto& GetString() const noexcept { return Get().m_sString; }

	[[nodiscard]] std::size_t Length() const noexcept;

protected:
	CStringContent m_oValue;
};

class CStringValue final : public CValue<CInternalStringValue>
{
public:
	CStringValue() = default;

	[[nodiscard]] static CStringValue* Construct(CProgramRuntime* const runtime, const std::string& v);
	[[nodiscard]] static std::unique_ptr<struct BuiltInMethod_t> ConstructMethods(class CProgramInformation* const info);
	[[nodiscard]] static std::unique_ptr<struct BuiltInProperty_t> ConstructProperties(class CProgramInformation* const info);

	[[nodiscard]] EValueType Type() const noexcept override { return t_string; };
	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }

	[[nodiscard]] bool ToBoolean() const override { return !Internal()->GetString().empty(); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(ToBoolean()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(ToBoolean()); }
	[[nodiscard]] const std::string& ToString() const override { return Internal()->GetString(); }

	[[nodiscard]] CStringValue* ToCString() override { return this; }

	[[nodiscard]] CInternalStringValue* Internal();
	[[nodiscard]] const CInternalStringValue* Internal() const;

	[[nodiscard]] IValue* Index(IValue* index) override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "string"s; }
	[[nodiscard]] std::string ValueAsString() const override { return Internal()->GetString(); }

};
