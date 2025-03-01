#include "console.hpp"
#include "varjus_api/types/object.hpp"

#include "varjus_api/internal/structure.hpp"

#include <iostream>

FORWARD_DECLARE_METHOD(LogConsole);

void CConsoleValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod("log", LogConsole, UNCHECKED_PARAMETER_COUNT);
}

DEFINE_METHOD(LogConsole, args)
{
	std::string p;

	for (auto& v : args)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return IValue::Construct(ctx->m_pRuntime);
}