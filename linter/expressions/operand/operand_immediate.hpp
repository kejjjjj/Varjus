#pragma once

#include "operand.hpp"

struct CImmediateOperand final : public IOperand
{
	VARJUS_NONCOPYABLE(CImmediateOperand);
	CImmediateOperand(const CToken* token) : m_pToken(token) {}
	~CImmediateOperand() = default;

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_immediate;
	}

	[[nodiscard]] ASTNode ToAST() override;

private:
	[[nodiscard]] Varjus::EValueType GetImmediateType() const noexcept;
	[[nodiscard]] std::size_t GetImmediateSize() const noexcept;
	[[nodiscard]] VarjusString ToData() const noexcept;

	const CToken* m_pToken{};
};

