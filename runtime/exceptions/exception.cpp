#include "exception.hpp"
#include "runtime/runtime.hpp"

CRuntimeError::CRuntimeError(const std::string& error)
    : m_oErrorMessage(error) {

    auto& [l, c] = *CProgramRuntime::GetExecutionPosition();
    m_oErrorMessage += std::format(" | near [{}, {}]", l, c);

}
