#include "error.hpp"

CLinterError::CLinterError(const std::string& error)
    : m_oErrorMessageFormatted(error) {

    if (!CLinterErrors::GetExecutionPosition()) {
        m_oErrorMessageFormatted += std::format(" | near beginning");
        return;
    }

    auto& [l, c] = *CLinterErrors::GetExecutionPosition();
    m_oErrorMessageFormatted += std::format(" | near [{}, {}]", l, c);
}

void CLinterErrors::SetExecutionPosition(const CodePosition* pos) noexcept {
    m_pCodePosition = pos;
}
const CodePosition* CLinterErrors::GetExecutionPosition() noexcept {
    return m_pCodePosition;
}
void CLinterErrors::PushError(const CLinterError& error) {
    errorStack.push_back(error);
    throw errorStack.back();

}
void CLinterErrors::PushError(const std::string& error, const CodePosition& position) {
    errorStack.emplace_back(CLinterError(error, position));
    throw errorStack.back();
}
void CLinterErrors::PushError(const std::string& error) {
    errorStack.emplace_back(CLinterError(error));
    throw errorStack.back();
}
void CLinterErrors::ClearErrorStack() 
{ 
    errorStack.clear(); 
}
const auto& CLinterErrors::GetErrorStack() 
{
    return errorStack; 
}

std::vector<CLinterError> CLinterErrors::errorStack;
const CodePosition* CLinterErrors::m_pCodePosition;
