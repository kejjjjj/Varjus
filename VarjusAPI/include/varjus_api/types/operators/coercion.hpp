#pragma once
#include <tuple>

#include "varjus_api/types/default.hpp"

struct CCoercionOperands final
{
	CCoercionOperands(IValue* l, IValue* r, IValue* alloc, bool lhsIsWeak)
		: lhs(l), rhs(r), allocated(alloc), allocatedIsLHS(lhsIsWeak){}

	IValue* lhs;
	IValue* rhs;
	IValue* allocated;

	IValue* GetLHS() const noexcept {
		if (!allocated)
			return lhs;

		return allocatedIsLHS ? allocated : lhs;
	}
	IValue* GetRHS() const noexcept {

		if (!allocated)
			return rhs;

		return !allocatedIsLHS ? allocated : rhs;
	}

private:
	bool allocatedIsLHS = false;
};

[[nodiscard]] std::tuple<IValue*, IValue*, IValue*> Coerce(Varjus::CProgramRuntime* const runtime, IValue* lhs, IValue* rhs);
[[nodiscard]] CCoercionOperands CoerceInternal(Varjus::CProgramRuntime* const runtime, IValue* lhs, IValue* rhs, bool lhsIsWeak);
