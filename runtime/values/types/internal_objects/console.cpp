#include "console.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"
#include <iostream>

FORWARD_DECLARE_METHOD(LogConsole);

BuiltInMethod_t CConsoleValue::ConstructMethods()
{
	BuiltInMethod_t m_oMethods;
	ADD_METHOD("log", LogConsole, UNCHECKED_PARAMETER_COUNT);

	return m_oMethods;
}

DEFINE_METHOD(LogConsole)
{
	std::string p;

	for (auto& v : newValues)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return CProgramRuntime::AcquireNewValue<IValue>();
}