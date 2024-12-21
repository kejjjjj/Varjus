#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/method.hpp"

class CConsoleValue final : public CObjectValue, public CDataTypeMethods<CConsoleValue>
{
	using ConsoleMethods = CBuiltInMethods<CConsoleValue>::InputType;

public:

	CConsoleValue() : CObjectValue() {}

	static CConsoleValue* Construct();
	static ConsoleMethods ConstructMethods();

	void Release() override;
	IValue* Copy() override;

	[[nodiscard]] constexpr bool IsCallable() const noexcept override { return CDataTypeMethods::IsCallable(); }

	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;
	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;

private:
	[[nodiscard]] IValue* Log(CRuntimeContext* const ctx, const IValues& values);
	//[[nodiscard]] IValue* Clear(CRuntimeContext* const ctx, const IValues& values);
};
