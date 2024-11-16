#pragma once

#include "operand.hpp"

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

struct CKeyValue
{
	NONCOPYABLE(CKeyValue);

	CKeyValue(std::size_t k, UniqueAST&& ast);

	std::size_t m_uKey;
	UniqueAST m_pValue;
};

struct CObjectOperand final : public IOperand
{
	NONCOPYABLE(CObjectOperand);

	CObjectOperand() = default;
	CObjectOperand(VectorOf<KeyValue<std::size_t, UniqueAST>>&& ptr);
	~CObjectOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_object;
	}

	[[nodiscard]] UniqueAST ToAST() override;

	VectorOf<KeyValue<std::size_t, UniqueAST>> m_oAttributes;
};
