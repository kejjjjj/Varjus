#pragma once
#include <string>

class CProgramRuntime;

class CRuntimeError final : public std::exception
{
public:
    CRuntimeError(CProgramRuntime* const runtime, const std::string& error);

    [[nodiscard]] char const* what() const noexcept override {
        return m_oErrorMessage.size() ? m_oErrorMessage.c_str() : "Unknown exception";
    }

private:
    std::string m_oErrorMessage;
};