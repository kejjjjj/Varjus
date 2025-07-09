#include "stddate.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/structure.hpp"

#include "varjus_api/internal/exceptions/exception.hpp"

#include <chrono>

using namespace Varjus;

FORWARD_DECLARE_METHOD(DateNow);

void CStdDateValue::Construct(ObjectDeclaration_t& receiver)
{
	receiver.AddMethod(VSL("now"), DateNow, 0);
}


DEFINE_METHOD(DateNow, args)
{
    const auto time = static_cast<VarjusUInt>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());

    return CUIntValue::Construct(ctx->m_pRuntime, time);
}
