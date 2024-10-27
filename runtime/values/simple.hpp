#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>

class CVariable;

class IValue
{
public:
	IValue() = default;
	virtual ~IValue() = default;

	[[nodiscard]] virtual constexpr bool PassAsReference() const noexcept { return false; }
	[[nodiscard]] virtual EValueType Type() const noexcept { return t_undefined; };

	constexpr auto SetOwner(CVariable* o) noexcept { m_pOwner = o; }
	constexpr auto GetOwner() const noexcept { return m_pOwner; }
	constexpr auto HasOwner() const noexcept { return !!m_pOwner; }

	void Release();

protected:

	CVariable* m_pOwner{ nullptr };
};


