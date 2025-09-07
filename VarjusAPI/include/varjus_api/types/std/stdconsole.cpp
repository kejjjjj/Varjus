
#include "stdconsole.hpp"

#include "runtime/misc/defs.hpp"

#include "varjus_api/types/object.hpp"
#include "varjus_api/internal/structure.hpp"

#include <iostream>

using namespace Varjus;

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
	p.reserve(1024);

	for (auto& v : args) {
		VarjusString s = v->ValueAsEscapedString();
		if (p.size() + s.size() > p.capacity())
			p.reserve((p.size() + s.size()) * 2);
		p += s;
	}

	Varjus::fmt::print(std::cout, VSL("{}\n"), p);
	return IValue::Construct(ctx->m_pRuntime);
}
DEFINE_METHOD(LogConsoleError, args)
{
	VarjusString p;
	p.reserve(1024);

	for (auto& v : args) {
		VarjusString s = v->ValueAsEscapedString();
		if (p.size() + s.size() > p.capacity())
			p.reserve((p.size() + s.size()) * 2);
		p += s;
	}

	Varjus::fmt::print(std::cerr, VSL("{}\n"), p);
	return IValue::Construct(ctx->m_pRuntime);
}