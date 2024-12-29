#pragma once
#include <tuple>

#include "types/opt_value.hpp"

struct CCoercionOperands final
{
	CCoercionOperands(IConstEvalValue* l, IConstEvalValue* r, IConstEvalValue* alloc, bool lhsIsWeak)
		: lhs(l), rhs(r), allocated(alloc), allocatedIsLHS(lhsIsWeak) {
	}

	IConstEvalValue* lhs;
	IConstEvalValue* rhs;
	IConstEvalValue* allocated;

	IConstEvalValue* GetLHS() const noexcept {
		if (!allocated)
			return lhs;

		return allocatedIsLHS ? allocated : lhs;
	}
	IConstEvalValue* GetRHS() const noexcept {

		if (!allocated)
			return rhs;

		return !allocatedIsLHS ? allocated : rhs;
	}

private:
	bool allocatedIsLHS = false;
};

std::tuple<IConstEvalValue*, IConstEvalValue*, IConstEvalValue*> Coerce(IConstEvalValue* lhs, IConstEvalValue* rhs);
CCoercionOperands CoerceInternal(IConstEvalValue* lhs, IConstEvalValue* rhs, bool lhsIsWeak);
