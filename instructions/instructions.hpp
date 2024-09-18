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

	inline void AddOperand(UniqueOperand&& op) { m_oOperands.emplace_back(std::forward<UniqueOperand&&>(op)); }

protected:
	Instruction m_eInstruction{ NOP };
	std::vector<UniqueOperand> m_oOperands;
};
