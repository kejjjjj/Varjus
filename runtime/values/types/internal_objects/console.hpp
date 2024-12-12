#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/builtin_methods.hpp"

class CConsoleValue final : public CObjectValue
{
	using ConsoleMethods = CBuiltInMethods<CConsoleValue>::InputType;

public:

	CConsoleValue() : CObjectValue() {}

	static CConsoleValue* Construct();
	static ConsoleMethods ConstructMethods();

	void Release() override;
	CConsoleValue* Copy() override;

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return !!m_pMethod; }

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;
	[[nodiscard]] IValue* Call(CFunction* const thisFunction, const IValues& args) override;

private:
	[[nodiscard]] IValue* Log(CFunction* const thisFunction, const IValues& values);

	const CBuiltInMethod<CConsoleValue>* m_pMethod{ nullptr };
};
