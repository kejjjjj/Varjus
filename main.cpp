#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]


#if UNICODE
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{

    if (argc != 2) {
        STD_COUT << VSL("usage: <file path>\n");
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

    if (!state.LoadScriptFromFile(argv[1])) {
        STD_CERR << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    if (const auto returnValue = state.ExecuteScript()) {
        STD_COUT << VSL("the program returned: ") << returnValue->ToPrintableString() << std::endl;
    }
    else {
        STD_CERR << VSL("runtime error: ") << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    return 0;
}