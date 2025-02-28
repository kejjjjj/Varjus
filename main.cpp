#include <iostream>

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"

#include "varjus_api/varjus_api.hpp"

#include "varjus_api/internal/runtime.hpp"

int ExitApp(int v)
{
    std::cout << "Press ENTER to exit\n";
    std::cin.get();
    return v;
}

int main()
{
    
    Varjus::State state;

    const auto reader = VarjusIOReader(DIRECTORY_SEPARATOR_CHAR + "scripts"s + DIRECTORY_SEPARATOR_CHAR + "script.var"s);

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!state.UseStdLibrary()) {
        std::cout << "state error: " << GetError(state.GetErrorMessage()) << '\n';
        return ExitApp(0);
    }

    if (!state.LoadScriptFromFile(reader.GetFilePath())) {
        std::cout << "syntax error: " << GetError(state.GetErrorMessage()) << '\n';
        return ExitApp(0);
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << '\n';

        returnValue->GetAllocator()->PrintAllLeaks();
    } else {
        std::cout << "runtime error: " << GetError(state.GetErrorMessage()) << '\n';
    }

    return ExitApp(1);
}

