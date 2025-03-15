#include "console.hpp"
#include "varjus_api/types/object.hpp"

#include "varjus_api/internal/structure.hpp"

#include <iostream>

FORWARD_DECLARE_METHOD(LogConsole);

void CConsoleValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("log"), LogConsole, UNCHECKED_PARAMETER_COUNT);
}

DEFINE_METHOD(LogConsole, args)
{
	VarjusString p;

	for (auto& v : args)
		p += v->ValueAsString();

	fmt::print(STD_COUT, VSL("{}"), p);
	return IValue::Construct(ctx->m_pRuntime);
}