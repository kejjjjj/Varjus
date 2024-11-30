#include "exception.hpp"
#include "runtime/runtime.hpp"

CRuntimeError::CRuntimeError(const std::string& error)
    : m_oErrorMessage(error) {

    if (!CProgramRuntime::GetExecutionPosition())
        return;

    auto& [l, c] = *CProgramRuntime::GetExecutionPosition();
    m_oErrorMessage += std::format(" | near [{}, {}]", l, c);
}
