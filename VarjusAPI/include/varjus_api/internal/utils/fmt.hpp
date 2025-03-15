#pragma once

#include <string>
#include <vector>
#include <iostream>

#ifdef UNICODE
using VarjusString = std::u16string;  // Use std::wstring if UNICODE is defined
using VarjusChar = char16_t;  //DANGER !!!
using VarjusUChar = char16_t; //DANGER !!!


#define VSL(str) u##str  // Prefix with U for wide strings
#define STD_COUT std::wcout
#define STD_CERR std::wcerr
#define STD_OSTREAM std::wostream
#define STD_IFSTREAM std::ifstream
#define STD_OFSTREAM std::wofstream
#define STD_STRING_VIEW std::u16string_view
#else
using VarjusString = std::string;   // Use VarjusString if UNICODE is not defined
using VarjusChar = char;
using VarjusUChar = unsigned char;

#define VSL(str) str     // No prefix for regular strings
#define STD_COUT std::cout
#define STD_CERR std::cerr
#define STD_OSTREAM std::ostream
#define STD_IFSTREAM std::ifstream
#define STD_OFSTREAM std::ofstream
#define STD_STRING_VIEW std::string_view
#endif

struct LocaleConverter
{
    [[nodiscard]] static std::u16string utf8_to_u16string(const std::string& string);
    [[nodiscard]] static std::u16string utf16le_to_u16string(const std::string& utf16le_data);
    [[nodiscard]] static std::u16string utf16be_to_u16string(const std::string& utf16be_data);
    [[nodiscard]] static std::string u16string_to_utf8(const std::u16string& u16str);
    [[nodiscard]] static std::wstring u16string_to_wstring(const std::u16string& u16str);
    [[nodiscard]] static std::u16string wstring_to_u16string(const std::wstring& str);

};

template <typename T>
void formatArg(std::ostringstream& oss, const T& arg) {
    oss << arg;
}

namespace fmt {

    template<typename T> /*requires std::is_arithmetic_v<T>*/
    [[nodiscard]] inline VarjusString to_string(T val) {

        if constexpr (std::is_convertible_v<T, VarjusString>)
            return val;

#ifdef UNICODE
        else if constexpr (std::is_convertible_v<const char*, T>)
            return LocaleConverter::utf8_to_u16string(val);
        else
            return LocaleConverter::utf8_to_u16string(std::to_string(val));
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
    inline void print(STD_OSTREAM& out, const VarjusString& _fmt, Args&&... args) {
#ifdef UNICODE
        out << LocaleConverter::u16string_to_wstring(fmt::format(_fmt, std::forward<Args&&>(args)...));
#else
        out << fmt::format(_fmt, std::forward<Args&&>(args)...);
#endif
    }


}


