#pragma once

#include <string>
#include <vector>
#include <sstream>

using CodePosition = std::tuple<size_t, size_t>;

class CModule;

class CLinterError final : public std::exception
{
public:
	CLinterError(const std::string& filePath, const std::string& error, const CodePosition& position)
        : m_oSourcePosition(position) {
		m_oErrorMessageFormatted = GetStringFormatted(filePath, error);
	}
    CLinterError(const std::string& filePath, const std::string& error, const CodePosition* pos);

    [[nodiscard]] char const* what() const noexcept override {
        return m_oErrorMessageFormatted.size() ? m_oErrorMessageFormatted.c_str() : "Unknown exception";
    }

private:
    std::string GetStringFormatted(const std::string& filePath, const std::string& err) const noexcept {

        std::stringstream ss;
        ss << err << "\nAt: " << std::get<0>(m_oSourcePosition) << ':' << std::get<1>(m_oSourcePosition)
            << " in \"" << filePath << "\"";
        return ss.str();
    }

    std::string m_oErrorMessageFormatted;

	CodePosition m_oSourcePosition;
};

class CLinterErrors final {

public:


    void PushError(const CLinterError& error);
    void PushError(const std::string& filePath, const std::string& error, const CodePosition& position);
    void PushError(const std::string& filePath, const std::string& error);

    void ClearErrorStack() { errorStack.clear(); }
    [[nodiscard]] constexpr std::vector<CLinterError>& GetErrorStack() { return errorStack; }

private:
    std::vector<CLinterError> errorStack;

};


//std::vector<CLinterError> CLinterErrors::errorStack = {};
