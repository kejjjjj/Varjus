#pragma once

#include "operand.hpp"
#include <variant>
#include <string>

struct FmtStringAST;

struct CFmtStringOperand final : public IOperand
{

	NONCOPYABLE(CFmtStringOperand);

	CFmtStringOperand() = default;
	CFmtStringOperand(VectorOf<FmtStringAST>&& nodes);
	~CFmtStringOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_fmt_string;
	}

	[[nodiscard]] ASTNode ToAST() override;


private:
	VectorOf<FmtStringAST> m_oNodes;
};
