#include "error.hpp"
#include "linter/modules/module.hpp"

#include <format>

CLinterError::CLinterError(const std::string& filePath, const std::string& error, const CodePosition* pos)
    : m_oErrorMessageFormatted(error) {

    if (!pos) {
        return;
    }

    auto& [l, c] = *pos;
    m_oErrorMessageFormatted += std::format(" | near [{}, {}] in \"{}\"", l, c, filePath);
}

void CLinterErrors::PushError(const CLinterError& error) {
    errorStack.push_back(error);
    throw errorStack.back();

}
void CLinterErrors::PushError(const std::string& filePath, const std::string& error, const CodePosition& position) {
    errorStack.emplace_back(CLinterError(filePath, error, position));
    throw errorStack.back();
}
void CLinterErrors::PushError(const std::string& filePath, const std::string& error) {
    errorStack.emplace_back(CLinterError(filePath, error, nullptr));
    throw errorStack.back();
}