#include "stdconsole.hpp"
#include "varjus_api/types/object.hpp"

#include "varjus_api/internal/structure.hpp"

#include <iostream>

FORWARD_DECLARE_METHOD(LogConsole);
FORWARD_DECLARE_METHOD(LogConsoleError);

void CConsoleValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("log"), LogConsole, UNCHECKED_PARAMETER_COUNT);
	receiver.AddMethod(VSL("error"), LogConsoleError, UNCHECKED_PARAMETER_COUNT);
}

DEFINE_METHOD(LogConsole, args)
{
	VarjusString p;

	for (auto& v : args)
		p += v->ValueAsEscapedString();

	fmt::print(std::cout, VSL("{}\n"), p);
	return IValue::Construct(ctx->m_pRuntime);
}
DEFINE_METHOD(LogConsoleError, args)
{
	VarjusString p;

	for (auto& v : args)
		p += v->ValueAsEscapedString();

	fmt::print(std::cerr, VSL("{}\n"), p);
	return IValue::Construct(ctx->m_pRuntime);
}