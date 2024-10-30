#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>
#include <string>

using namespace std::string_literals;

class CVariable;

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
	void Print() const;

	[[nodiscard]] virtual std::string TypeAsString() const { return "undefined"s; }
	[[nodiscard]] virtual std::string ValueAsString() const { return "undefined"s; }
	
	[[nodiscard]] virtual IValue* Copy() const;

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] std::int64_t& AsInt();
	[[nodiscard]] double &AsDouble();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }
	[[nodiscard]] virtual double ToDouble() const { return 0.0; }

protected:

	CVariable* m_pOwner{ nullptr };
};


