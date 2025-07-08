#pragma once

#include "operand.hpp"

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

struct CKeyValue
{
	VARJUS_NONCOPYABLE(CKeyValue);

	CKeyValue(std::size_t k, ASTNode&& ast);

	std::size_t m_uKey;
	ASTNode m_pValue;
};

struct CObjectOperand final : public IOperand
{
	VARJUS_NONCOPYABLE(CObjectOperand);

	CObjectOperand() = default;
	CObjectOperand(VectorOf<KeyValue<std::size_t, ASTNode>>&& ptr);
	~CObjectOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_object;
	}

	[[nodiscard]] ASTNode ToAST() override;

	VectorOf<KeyValue<std::size_t, ASTNode>> m_oAttributes;
};
