#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete

#if defined(_MSC_VER) 
#define WARNING_PUSH() __pragma(warning(push))
#define WARNING_DISABLE(w) __pragma(warning(disable : w))
#define WARNING_POP() __pragma(warning(pop))
#else
#define WARNING_PUSH()
#define WARNING_POP()
#define WARNING_DISABLE(v)
#endif 

#ifdef _WIN32
#ifdef _WIN64
// 64-bit Windows
using VarjusInt = long long;
using VarjusUInt = unsigned long long;
#else
// 32-bit Windows
using VarjusInt = int;
using VarjusUInt = unsigned int;
#endif
#else
#if __x86_64__ || __ppc64__
// 64-bit non-Windows
using VarjusInt = long long;
using VarjusUInt = unsigned long long;
#else
// 32-bit non-Windows
using VarjusInt = int;
using VarjusUInt = unsigned int;
#endif
#endif

using VarjusDouble = double;

#include <string>

#ifdef UNICODE
#define VSL(str) L##str  // Prefix with L for wide strings
#define STD_TO_STRING(str) std::to_wstring(str)
#define STD_COUT std::wcout
#define STD_CERR std::wcerr
#define STD_STRINGSTREAM std::wstringstream
#define STD_IFSTREAM std::wifstream
#define STD_OFSTREAM std::wofstream
#define STD_STRING_VIEW std::wstring_view
using VarjusString = std::wstring;  // Use std::wstring if UNICODE is defined
using VarjusChar = wchar_t;
using VarjusUChar = wchar_t;

#else
#define VSL(str) str     // No prefix for regular strings
#define STD_TO_STRING(str) std::to_string(str)
#define STD_COUT std::cout
#define STD_CERR std::cerr
#define STD_STRINGSTREAM std::stringstream
#define STD_IFSTREAM std::ifstream
#define STD_OFSTREAM std::ofstream
#define STD_STRING_VIEW std::string_view
using VarjusString = std::string;   // Use VarjusString if UNICODE is not defined
using VarjusChar = char;
using VarjusUChar = unsigned char;

#endif