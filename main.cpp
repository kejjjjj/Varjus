#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]

#ifdef UNICODE
#define _UC 1
#else
#define _UC 0
#endif

#ifdef _MSC_VER
#if _UC
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    if (argc != 2) {
        fmt::print(STD_CERR, VSL("usage: <file path>\n"));
        return 0;
    }

    Varjus::State state;

    const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
        return errorMsg ? *errorMsg : VSL("unknown error!");
    };

    if (!state.UseStdLibrary()) {
        fmt::print(STD_CERR, VSL("state error: {}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }

#if _UC
    if (!state.LoadScriptFromFile(reinterpret_cast<char16_t*>(argv[1]), e_auto)) {
        fmt::print(STD_CERR, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }
#else
    if (!state.LoadScriptFromFile(argv[1], e_auto)) {
        fmt::print(STD_CERR, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }
#endif

    if (const auto returnValue = state.ExecuteScript()) {
        fmt::print(STD_COUT, VSL("the program returned: {}\n"), returnValue->ToPrintableString());
    }
    else {
        fmt::print(STD_CERR, VSL("runtime error: {}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }

    return 0;
}

#else

#if _UC
#include <cwchar>

std::vector<VarjusString> ConvertArgvToWide(int argc, char** argv)
{
    std::vector<VarjusString> wide_argv(static_cast<std::size_t>(argc));

    //ignore first argv
    for (auto i = 0; i < argc; i++) {
        wide_argv[i] = LocaleConverter::utf8_to_u16string(argv[i]);
    }

    return wide_argv;
}

#endif

int main(int argc, char** argv)
{
    if (argc != 2) {
        fmt::print(STD_CERR, VSL("usage: <file path>\n"));
        return 0;
    }

    Varjus::State state;

    const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
        return errorMsg ? *errorMsg : VSL("unknown error!");
    };

    if (!state.UseStdLibrary()) {
        fmt::print(STD_CERR, VSL("state error: {}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }

#if _UC
    auto argvs = ConvertArgvToWide(argc, argv);

    for(const auto& s : argvs)
        fmt::print(STD_COUT, VSL("{}\n"), s);

    if (argvs.size() < 2) {
        fmt::print(STD_CERR, VSL("couldn't convert argv to unicode\n"));
        return 0;
    }
    if (!state.LoadScriptFromFile(argvs[1], e_auto)) {
        fmt::print(STD_CERR, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }

#else
    if (!state.LoadScriptFromFile(argv[1], e_auto)) {
        fmt::print(STD_CERR, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }
#endif

    if (const auto returnValue = state.ExecuteScript()) {
        fmt::print(STD_COUT, VSL("the program returned: {}\n"), returnValue->ToPrintableString());
    }
    else {
        fmt::print(STD_CERR, VSL("runtime error: {}\n"), GetError(state.GetErrorMessage()));
        return 0;
    }

    return 0;
}

#endif
