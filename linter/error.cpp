#include "error.hpp"
#include "linter/modules/module.hpp"

#include <format>

CLinterError::CLinterError(const VarjusString& filePath, const VarjusString& error, const CodePosition* pos)
    : m_oErrorMessageFormatted(error) {

    if (!pos) {
        return;
    }

    auto& [l, c] = *pos;
    m_oErrorMessageFormatted += fmt::format(VSL(" | near [{}, {}] in \"{}\""), l, c, filePath);
}

void CLinterErrors::PushError(const CLinterError& error) {
    errorStack.push_back(error);
    throw errorStack.back();

}
void CLinterErrors::PushError(const VarjusString& filePath, const VarjusString& error, const CodePosition& position) {
    errorStack.emplace_back(CLinterError(filePath, error, position));
    throw errorStack.back();
}
void CLinterErrors::PushError(const VarjusString& filePath, const VarjusString& error) {
    errorStack.emplace_back(CLinterError(filePath, error, nullptr));
    throw errorStack.back();
}