#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>
#include <string>

using namespace std::string_literals;

class CVariable;
class CRuntimeFunction;

inline std::string emptyString;

class IValue
{
public:
	IValue() = default;
	virtual ~IValue() = default;

	[[nodiscard]] virtual constexpr bool PassAsReference() const noexcept { return false; }
	[[nodiscard]] virtual EValueType Type() const noexcept { return t_undefined; };

	constexpr auto SetOwner(CVariable* o) noexcept { m_pOwner = o; }
	[[nodiscard]] constexpr auto GetOwner() const noexcept { return m_pOwner; }
	[[nodiscard]] constexpr auto HasOwner() const noexcept { return !!m_pOwner; }

	virtual void Release();

	[[nodiscard]] std::string ToPrintableString() const;

	[[nodiscard]] virtual std::string TypeAsString() const { return "undefined"s; }
	[[nodiscard]] virtual std::string ValueAsString() const { return "undefined"s; }
	
	[[nodiscard]] virtual IValue* Copy() const;

	constexpr void MakeImmutable() noexcept { m_bIsConst = true; }

	[[nodiscard]] constexpr virtual bool IsImmutable() const noexcept { return m_bIsConst; }
	[[nodiscard]] constexpr virtual bool IsCoerceable() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsArithmetic() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsIntegral() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsIndexable() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsCallable() const noexcept { return false; }

	[[nodiscard]] virtual IValue* Index(std::int64_t index);

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] std::int64_t& AsInt();
	[[nodiscard]] double &AsDouble();
	[[nodiscard]] std::string& AsString();
	[[nodiscard]] CRuntimeFunction* AsCallable();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }
	[[nodiscard]] virtual double ToDouble() const { return 0.0; }
	[[nodiscard]] virtual const std::string& ToString() const { return emptyString; }

protected:
	void ReleaseInternal();
	void RemoveConstness() noexcept { m_bIsConst = false; }

	CVariable* m_pOwner{ nullptr };
	bool m_bIsConst{ false };
};


template<typename Value>
class CValue : public IValue
{
	friend class IValue;
public:
	CValue() = default;
	CValue(const Value& v) : m_oValue(v) {}
	virtual ~CValue() = default;

	Value& GetRawValue() noexcept { return m_oValue; }
	void SetRawValue(const Value&v) noexcept { m_oValue = v; }

protected:
	Value m_oValue{};
};
