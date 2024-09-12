#pragma once
#include <vector>
#include <string>

#include "operand.hpp"

enum Instruction : std::uint8_t
{
	NOP,
	MOV,
	ADD
};

class COperation
{
public:
	constexpr COperation(Instruction insn, const std::vector<COperand>& operands) : m_eInstruction(insn) {}
	~COperation() = default;

	[[nodiscard]] constexpr Instruction GetInstruction() const noexcept { return m_eInstruction; }

protected:
	Instruction m_eInstruction{ NOP };
	std::vector<COperand> m_oOperands;
};
