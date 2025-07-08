#include "exception.hpp"
#include "varjus_api/internal/runtime.hpp"

using namespace Varjus;

CRuntimeError::CRuntimeError(Varjus::CProgramRuntime* const runtime, const VarjusString& error)
    : m_oErrorMessage(error) {

    assert(runtime);

    if (!runtime->GetExecutionPosition()) {
        m_oErrorMessage += Varjus::fmt::format(VSL(" | near main function"));
        return;
    }

    auto& [l, c] = *runtime->GetExecutionPosition();
    m_oErrorMessage += Varjus::fmt::format(VSL(" | near [{}, {}]"), l, c);
}
