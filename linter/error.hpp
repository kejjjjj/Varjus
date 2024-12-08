#pragma once

#include <string>
#include <vector>
#include <sstream>

using CodePosition = std::tuple<size_t, size_t>;

class CLinterError final : public std::exception
{
public:
	CLinterError(const std::string& error, const CodePosition& position) 
        : m_oErrorMessage(error), m_oSourcePosition(position) {
		m_oErrorMessageFormatted = GetStringFormatted();
	}

    [[nodiscard]] char const* what() const noexcept override{
        return m_oErrorMessageFormatted.size() ? m_oErrorMessageFormatted.c_str() : "Unknown exception";
    }

private:
    std::string GetStringFormatted() const noexcept {

        std::stringstream ss;
        ss << "Error: " << m_oErrorMessage << "\nAt: " << std::get<0>(m_oSourcePosition) << ':' << std::get<1>(m_oSourcePosition);
        return ss.str();
    }

	std::string m_oErrorMessage;
    std::string m_oErrorMessageFormatted;

	CodePosition m_oSourcePosition;
};

class CLinterErrors final {

public:
    static void PushError(const CLinterError& error);
    static void PushError(const std::string& error, const CodePosition& position);
    static void ClearErrorStack();
    [[nodiscard ]] static const auto& GetErrorStack();

private:
    static std::vector<CLinterError> errorStack;

};

//std::vector<CLinterError> CLinterErrors::errorStack = {};
