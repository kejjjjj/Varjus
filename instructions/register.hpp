
#pragma once
#include "operand.hpp"

#include <string>
#include <array>

enum Register : std::uint8_t
{
	EAX,
	EBX,
	ECX,
	EDX,
};

struct CRegister {
	Register m_eRegister{};
	std::string_view m_sName{};
};

std::array<CRegister, 4u> registers = {
	{ 
		{ EAX, "eax" },
		{ EBX, "ebx" },
		{ ECX, "ecx" },
		{ EDX, "edx" } 
	}
};

class CRegisterOperand : public COperand
{
public:
	constexpr CRegisterOperand(Register reg) : m_oRegister(&registers[reg]) {}

	[[nodiscard]] constexpr OperandType Type() const noexcept override { return o_register; }
	[[nodiscard]] std::string ToString() const noexcept override { return m_oRegister->m_sName.data(); }

private:
	const CRegister* m_oRegister{};
};
