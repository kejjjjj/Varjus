#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]

#if UNICODE && !_MSC_VER
#include <cwchar>

std::vector<std::wstring> ConvertArgvToWide(int argc, char** argv)
{
    std::vector<std::wstring> wide_argv(static_cast<std::size_t>(argc));

    for(auto i = 0; i < argc; i++){
        const auto len = std::mbstowcs(nullptr, argv[i], 0);
        if(len == static_cast<std::size_t>(-1)){
            STD_COUT << VSL("couldn't convert argv to unicode\n");
            return {};
        }
        std::wstring wstr(len, VSL('\0'));
        std::mbstowcs(wstr.data(), argv[i], len);
        wide_argv[i] = std::move(wstr);
    }

    return wide_argv;
}

#endif

#if UNICODE && _MSC_VER
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{

    if (argc != 2) {
        STD_CERR << VSL("usage: <file path>\n");
        return 0;
    }



    Varjus::State state;

    const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
        return errorMsg ? *errorMsg : VSL("unknown error!");
    };

    if (!state.UseStdLibrary()) {
        STD_CERR << VSL("state error: ") << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    #if UNICODE && !_MSC_VER
    auto argvs = ConvertArgvToWide(argc, argv);

    if(argvs.size() < 2){
        STD_CERR << VSL("couldn't convert argv to unicode\n");
        return 0;
    }

    for(const auto& w : argvs)
        STD_COUT << "file: " << w << '\n';

    if (!state.LoadScriptFromFile(argvs[1])) {
        STD_CERR << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    #else
    if (!state.LoadScriptFromFile(argv[1])) {
        STD_CERR << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }
    #endif

    if (const auto returnValue = state.ExecuteScript()) {
        STD_COUT << VSL("the program returned: ") << returnValue->ToPrintableString() << std::endl;
    }
    else {
        STD_CERR << VSL("runtime error: ") << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    return 0;
}