#include "fmt.hpp"


#include <locale>

#ifdef _MSC_VER  // Microsoft Visual C++
#include <codecvt>
#pragma warning(push)
#pragma warning(disable : 4996) // Disable deprecated warnings for MSVC
#elif defined(__GNUC__) || defined(__clang__)  // GCC & Clang
#include <codecvt>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef _WIN32
#include <Windows.h>
#endif


std::u16string LocaleConverter::utf8_to_u16string(const std::string& utf8_str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.from_bytes(utf8_str);
}

std::u16string LocaleConverter::ansi_to_u16string(const std::string& string) {
#ifdef _WIN32
    // Windows: Convert from ANSI (system code page) to UTF-16
    int len = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, nullptr, 0);
    std::u16string result(static_cast<char16_t>(len - 1), 0);
    MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, reinterpret_cast<wchar_t*>(&result[0]), len);
    return result;
#else
    // Linux/macOS: Assume ANSI is UTF-8 and convert
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.from_bytes(string);
#endif
}
std::u16string LocaleConverter::utf16le_to_u16string(const std::string& utf16le_data) {
    if (utf16le_data.size() % 2 != 0) {
        throw std::runtime_error("Invalid UTF-16LE data (odd byte count)");
    }

    return std::u16string(reinterpret_cast<const char16_t*>(utf16le_data.data()), utf16le_data.size() / 2);
}
std::u16string LocaleConverter::utf16be_to_u16string(const std::string& utf16be_data) {
    if (utf16be_data.size() % 2 != 0) {
        throw std::runtime_error("Invalid UTF-16BE data (odd byte count)");
    }

    constexpr auto swap_endian = [](char16_t val) -> auto {
        return static_cast<char16_t>((val >> static_cast<char16_t>(8)) | (val << static_cast<char16_t>(8)));
    };

    std::u16string u16str(utf16be_data.size() / 2, u'\0');

    for (size_t i = 0; i < u16str.size(); ++i) {
        auto ch = *reinterpret_cast<const char16_t*>(&utf16be_data[2 * i]);
        u16str[i] = swap_endian(ch);  // Convert from BE to LE
    }

    return u16str;
}
std::string LocaleConverter::u16string_to_ansi(const std::u16string& u16str) {

    // Linux/macOS: Assume ANSI is UTF-8 and convert
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.to_bytes(u16str);

}
std::wstring LocaleConverter::u16string_to_wstring(const std::u16string& u16str) {
    return std::wstring(u16str.begin(), u16str.end());
}
std::u16string LocaleConverter::wstring_to_u16string(const std::wstring& str) {
    return std::u16string(str.begin(), str.end());
}
#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
