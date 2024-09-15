#pragma once

#include "operand.hpp"

using UniqueOperand = std::unique_ptr<COperand>;

enum Instruction : std::uint8_t
{
	NOP,
	MOV,
	ADD
};

class COperation
{
public:
	COperation(Instruction insn) :
		m_eInstruction(insn) {}
	~COperation() = default;

	[[nodiscard]] constexpr Instruction GetInstruction() const noexcept { return m_eInstruction; }

protected:
	Instruction m_eInstruction{ NOP };
	std::vector<UniqueOperand> m_oOperands;
};
