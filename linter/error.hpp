#pragma once


#include <vector>
#include <sstream>
#include <codecvt>
#include <locale>
#include <tuple>

#include "varjus_api/internal/globalDefinitions.hpp"


using __CodePosition = std::tuple<size_t, size_t>;

class CModule;

class CLinterError final
{
public:
	CLinterError(const VarjusString& filePath, const VarjusString& error, const __CodePosition& position)
        : m_oSourcePosition(position) {
		m_oErrorMessageFormatted = GetStringFormatted(filePath, error);
	}
    CLinterError(const VarjusString& filePath, const VarjusString& error, const __CodePosition* pos);

    [[nodiscard]] VarjusString what() const noexcept {
#ifdef UNICODE
        return m_oErrorMessageFormatted.size() ? m_oErrorMessageFormatted.c_str() : VSL("Unknown exception");
#else
        return m_oErrorMessageFormatted.size() ? m_oErrorMessageFormatted.c_str() : "Unknown exception";
#endif
    }

private:
    VarjusString GetStringFormatted(const VarjusString& filePath, const VarjusString& err) const noexcept {
        
        return Varjus::fmt::format(VSL("{}\nAt: {}:{} in \"{}\""), err, std::get<0>(m_oSourcePosition), 
            std::get<1>(m_oSourcePosition), filePath);
    }

    VarjusString m_oErrorMessageFormatted;
    mutable std::string m_oErrorMessageMultibyte;
	__CodePosition m_oSourcePosition;
};

class CLinterErrors final {

public:


    void PushError(const CLinterError& error);
    void PushError(const VarjusString& filePath, const VarjusString& error, const __CodePosition& position);
    void PushError(const VarjusString& filePath, const VarjusString& error);

    void ClearErrorStack() { errorStack.clear(); }
    [[nodiscard]] constexpr std::vector<CLinterError>& GetErrorStack() { return errorStack; }

private:
    std::vector<CLinterError> errorStack;

};


//std::vector<CLinterError> CLinterErrors::errorStack = {};
