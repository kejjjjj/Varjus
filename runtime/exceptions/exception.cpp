#include "exception.hpp"
#include "api/internal/runtime.hpp"

CRuntimeError::CRuntimeError(const std::string& error)
    : m_oErrorMessage(error) {

    if (!CProgramRuntime::GetExecutionPosition()) {
        m_oErrorMessage += std::format(" | near main function");
        return;
    }

    auto& [l, c] = *CProgramRuntime::GetExecutionPosition();
    m_oErrorMessage += std::format(" | near [{}, {}]", l, c);
}
