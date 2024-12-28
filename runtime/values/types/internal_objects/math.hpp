#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/methods.hpp"


class CMathValue final : public CObjectValue
{
public:

	CMathValue() : CObjectValue() {}

	static CMathValue* Construct();
	static void ConstructMethods(); //only called once during init

	void Release() override;
	IValue* Copy() override;

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:

	static DECLARE_BUILT_IN_METHODS m_oMethods;
};
