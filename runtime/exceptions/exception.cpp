#include "exception.hpp"
#include "api/internal/runtime.hpp"

CRuntimeError::CRuntimeError(CProgramRuntime* const runtime, const std::string& error)
    : m_oErrorMessage(error) {

    assert(runtime);

    if (!runtime->GetExecutionPosition()) {
        m_oErrorMessage += std::format(" | near main function");
        return;
    }

    auto& [l, c] = *runtime->GetExecutionPosition();
    m_oErrorMessage += std::format(" | near [{}, {}]", l, c);
}
