#pragma once

#include "varjus_api/internal/globalDefinitions.hpp"
#include <codecvt>
#include <locale>

class CProgramRuntime;

class CRuntimeError final
{
public:
    CRuntimeError(CProgramRuntime* const runtime, const VarjusString& error);

    [[nodiscard]] VarjusString what() const noexcept {
#ifdef UNICODE
        return m_oErrorMessage.size() ? m_oErrorMessage.c_str() : VSL("Unknown exception");
#else
        return m_oErrorMessage.size() ? m_oErrorMessage.c_str() : "Unknown exception";
#endif
    }

private:
    VarjusString m_oErrorMessage;
    mutable std::string m_oErrorMessageMultibyte; // For storing converted UTF-16 to UTF-8 string if needed
};
