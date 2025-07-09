#include "stdstring.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"

#include <chrono>

using namespace Varjus;

FORWARD_DECLARE_METHOD(StringFromCode);

void CStdStringValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("from_code"), StringFromCode, 1);
}


DEFINE_METHOD(StringFromCode, args)
{
	const auto _code = args[0];

	if(!_code->IsIntegral())
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("string.from_code expected an integral type, but got \"{}\""), _code->TypeAsString()));

	const auto asUint = _code->ToUInt();
	const auto code = static_cast<VarjusChar>(asUint);
	const VarjusString str(1, code);

    return CStringValue::Construct(ctx->m_pRuntime, str);
}
