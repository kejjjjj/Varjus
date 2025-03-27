#pragma once

#include "varjus_api/internal/globalDefinitions.hpp"
#include <string>

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
};
