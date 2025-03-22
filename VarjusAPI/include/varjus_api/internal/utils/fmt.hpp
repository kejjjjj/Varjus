#pragma once

#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
    #ifdef UNICODE
        #define VSL(str) u##str
        #define STD_STRING_VIEW std::u16string_view
        // Windows Unicode (UTF-16)
        using VarjusString = std::u16string;
        using VarjusChar = char16_t;
        using VarjusUChar = char16_t;
    #else
        #define VSL(str) str
        #define STD_STRING_VIEW std::string_view
        // Windows Multibyte (ANSI)
        using VarjusString = std::string;
        using VarjusChar = char;
        using VarjusUChar = unsigned char;
    #endif
#else
    #define STD_STRING_VIEW std::string_view
    #define VSL(str) str
    // Linux/macOS: UTF-8 (std::string)
    using VarjusString = std::string;  // UTF-8 on Linux/macOS
    using VarjusChar = char;
    using VarjusUChar = unsigned char;
#endif

struct LocaleConverter
{
    [[nodiscard]] static std::u16string utf8_to_u16string(const std::string& utf8_str);
    [[nodiscard]] static std::u16string ansi_to_u16string(const std::string& string);
    [[nodiscard]] static std::u16string utf16le_to_u16string(const std::string& utf16le_data);
    [[nodiscard]] static std::u16string utf16be_to_u16string(const std::string& utf16be_data);
    [[nodiscard]] static std::string u16string_to_ansi(const std::u16string& u16str);
    [[nodiscard]] static std::wstring u16string_to_wstring(const std::u16string& u16str);
    [[nodiscard]] static std::u16string wstring_to_u16string(const std::wstring& str);

};

namespace fmt {

    template<typename T> /*requires std::is_arithmetic_v<T>*/
    [[nodiscard]] inline VarjusString to_string(T val) {

        if constexpr (std::is_convertible_v<T, VarjusString>)
            return val;

    #ifdef _WIN32
        #ifdef UNICODE
        else if constexpr (std::is_convertible_v<const char*, T>)
            return LocaleConverter::ansi_to_u16string(val);
        else
            return LocaleConverter::ansi_to_u16string(std::to_string(val));
        #else
        else
            return std::to_string(val);
        #endif

    #else
        else
            return std::to_string(val);
    #endif
    }

    template <typename... Args>
    [[nodiscard]] inline VarjusString format(const VarjusString& _fmt, Args&&... args) {

        std::vector<VarjusString> values = { fmt::to_string(args)... };
        size_t argIndex = 0;
        VarjusString result;

        for (size_t i = 0; i < _fmt.size(); ++i) {
            if (_fmt[i] == VSL('{') && i + 1 < _fmt.size() && _fmt[i + 1] == VSL('}')) {
                if (argIndex < values.size()) {
                    result += values[argIndex++];
                }
                i++; // Skip '}'
            }
            else {
                result += VarjusString(1, _fmt[i]);
            }
        }
        return result;
    }
    
    template<typename ... Args>
    inline void print(auto& out, const VarjusString& _fmt, Args&&... args) {

#ifdef _WIN32
    #ifdef UNICODE
        out << LocaleConverter::u16string_to_ansi(fmt::format(_fmt, std::forward<Args&&>(args)...));
    #else
        out << fmt::format(_fmt, std::forward<Args&&>(args)...);
    #endif
#else
        out << fmt::format(_fmt, std::forward<Args&&>(args)...);
#endif

    }


}


