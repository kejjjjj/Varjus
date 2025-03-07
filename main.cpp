#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]


int main(int argc, char** argv)
{

    if (argc != 2) {
        std::cout << "usage: <file path>\n";
        return 0;
    }

    Varjus::State state;

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!state.UseStdLibrary()) {
        std::cerr << "state error: " << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    if (!state.LoadScriptFromFile(argv[1])) {
        std::cerr << "syntax error: " << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << std::endl;
    }
    else {
        std::cerr << "runtime error: " << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    return 0;
}