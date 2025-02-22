#include "console.hpp"
#include "api/types/object.hpp"

#include "api/internal/structure.hpp"

#include <iostream>

FORWARD_DECLARE_METHOD(LogConsole);

BuiltInMethod_t CConsoleValue::ConstructMethods()
{
	BuiltInMethod_t m_oMethods;
	m_oMethods.AddMethod("log", LogConsole, UNCHECKED_PARAMETER_COUNT);

	return m_oMethods;
}

DEFINE_METHOD(LogConsole, args)
{
	std::string p;

	for (auto& v : args)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return IValue::Construct();
}