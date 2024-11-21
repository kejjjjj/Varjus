#pragma once

#include "runtime/values/types/simple.hpp"
#include "runtime/values/types/array.hpp"

#include <unordered_map>
#include <string>

struct CProgramContext;

using ElementIndex = std::size_t;

struct CArrayBuiltInMethod
{
	std::size_t m_uNumArguments;
	IValue* (CArrayValue::* memFn)(const IValues& values);
};


struct CStaticArrayBuiltInMethods
{
	static void Initialize(const CProgramContext* context);

	[[nodiscard]] static const CArrayBuiltInMethod* LookupMethod(ElementIndex index);

	[[nodiscard]] static IValue* CallMethod(CArrayValue* _this, const IValues& args, const CArrayBuiltInMethod* method);

	[[nodiscard]] static auto& GetIterator() noexcept { return m_oMethodLookup; }

private:
	static std::unordered_map<ElementIndex, const CArrayBuiltInMethod*> m_oMethodLookup;
	static std::unordered_map<std::string, CArrayBuiltInMethod> m_oStaticMethods;
};
