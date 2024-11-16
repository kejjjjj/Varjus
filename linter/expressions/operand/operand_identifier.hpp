#pragma once

#include "operand.hpp"

struct CLinterFunction;
struct CLinterVariable;
struct CMemoryIdentifier;

struct CIdentifierOperand final : public IOperand
{
	NONCOPYABLE(CIdentifierOperand);
	CIdentifierOperand(CMemoryIdentifier* id) : m_pIdentifier(id) {}
	~CIdentifierOperand() = default;

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_identifier;
	}

	[[nodiscard]] UniqueAST ToAST() override;

private:
	[[nodiscard]] CLinterVariable* GetVariable();
	[[nodiscard]] CLinterFunction* GetFunction();

	CMemoryIdentifier* m_pIdentifier;
};
