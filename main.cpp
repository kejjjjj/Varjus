#include <iostream>
#include "varjus_api/varjus_api.hpp"

int main()
{

    Varjus::State state;

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
        };

    if (!state.UseStdLibrary()) {
        std::cout << "state error: " << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    if (!state.LoadScriptFromFile("C:\\Temp\\script.var")) {
        std::cout << "syntax error: " << GetError(state.GetErrorMessage()) << '\n';
        return 0;
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << '\n';
    }
    else {
        std::cout << "runtime error: " << GetError(state.GetErrorMessage()) << '\n';
    }

    return 1;
}