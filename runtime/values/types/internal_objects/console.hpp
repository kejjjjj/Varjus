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
	[[nodiscard]] IValue* Call(CRuntimeContext* const ctx, const IValues& args) override;

private:
	[[nodiscard]] IValue* Log(CRuntimeContext* const ctx, const IValues& values);
	//[[nodiscard]] IValue* Clear(CRuntimeContext* const ctx, const IValues& values);

	const CBuiltInMethod<CConsoleValue>* m_pMethod{ nullptr };
};
