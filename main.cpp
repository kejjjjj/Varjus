#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]

#ifdef UNICODE
#define _UC 1
#else
#define _UC 0
#endif

void AddArgs(CProgramRuntime* const ctx, IValues& receiver)
{
    receiver.push_back(CIntValue::Construct(ctx, 420));
    receiver.push_back(CStringValue::Construct(ctx, VSL("Hello!")));

    receiver.push_back(CArrayValue::Construct(ctx, { 
        CIntValue::Construct(ctx, 1),
        CStringValue::Construct(ctx, VSL("aaaaaaaaaa!")) 
    }));

}

#ifdef _WIN32
#if _UC
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    if (argc != 2) {
        fmt::print(std::cerr, VSL("usage: <file path>\n"));
        return 0;
    }
    Varjus::State state;

    const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
        return errorMsg.value_or(VSL("unknown error!"));
        };

    if (!state.UseStdLibrary()) {
        fmt::print(std::cerr, VSL("state error: {}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }

#if _UC
    if (!state.LoadScriptFromFile(reinterpret_cast<char16_t*>(argv[1]), e_utf8)) {
        fmt::print(std::cerr, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }
#else
    if (!state.LoadScriptFromFile(argv[1], e_utf8)) {
        fmt::print(std::cerr, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }
#endif
    if (const auto returnValue = state.ExecuteScript(AddArgs)) {
        fmt::print(std::cout, VSL("the program returned: {}\n"), returnValue->ToPrintableString());
    }
    else {
        fmt::print(std::cerr, VSL("runtime error: {}\n"), GetError(state.GetErrorMessage()));
        return 1;
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
        wide_argv[i] = LocaleConverter::ansi_to_u16string(argv[i]);
    }

    return wide_argv;
}

#endif

int main(int argc, char** argv)
{
    if (argc != 2) {
        fmt::print(std::cerr, VSL("usage: <file path>\n"));
        return 1;
    }

    Varjus::State state;

    const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
        return errorMsg ? *errorMsg : VSL("unknown error!");
        };

    if (!state.UseStdLibrary()) {
        fmt::print(std::cerr, VSL("state error: {}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }

#if _UC
    auto argvs = ConvertArgvToWide(argc, argv);

    if (argvs.size() < 2) {
        fmt::print(std::cerr, VSL("couldn't convert argvs to char16_t strings\n"));
        return 1;
    }
    if (!state.LoadScriptFromFile(argvs[1], e_utf8)) {
        fmt::print(std::cerr, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }

#else
    if (!state.LoadScriptFromFile(argv[1], e_utf8)) {
        fmt::print(std::cerr, VSL("{}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }
#endif

    if (const auto returnValue = state.ExecuteScript()) {
        fmt::print(std::cout, VSL("the program returned: {}\n"), returnValue->ToPrintableString());
    }
    else {
        fmt::print(std::cerr, VSL("runtime error: {}\n"), GetError(state.GetErrorMessage()));
        return 1;
    }

    return 0;
}

#endif