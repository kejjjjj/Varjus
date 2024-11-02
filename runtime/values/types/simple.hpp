#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>
#include <string>

using namespace std::string_literals;

class CVariable;

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

	void Release();
	std::string ToPrintableString() const;

	[[nodiscard]] virtual std::string TypeAsString() const { return "undefined"s; }
	[[nodiscard]] virtual std::string ValueAsString() const { return "undefined"s; }
	
	[[nodiscard]] virtual IValue* Copy() const;

	[[nodiscard]] constexpr virtual bool Coerceable() const noexcept { return true; }
	[[nodiscard]] constexpr virtual bool IsArithmetic() const noexcept { return true; }

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] std::int64_t& AsInt();
	[[nodiscard]] double &AsDouble();
	[[nodiscard]] std::string& AsString();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }
	[[nodiscard]] virtual double ToDouble() const { return 0.0; }
	[[nodiscard]] virtual const std::string& ToString() const { return emptyString; }

protected:

	CVariable* m_pOwner{ nullptr };
};


