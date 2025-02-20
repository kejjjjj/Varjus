#include <iostream>

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"

#include "api/varjus_api.hpp"

int ExitApp(int v)
{
    Varjus::Cleanup();
#if _DEBUG
    Varjus::PrintMemoryUsage();
#endif
    std::cout << "Press ENTER to exit\n";
    std::cin.get();
    return v;
}

int main()
{
    Varjus::UseStdLibrary();

    const auto reader = VarjusIOReader("scripts\\script.var");
    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!Varjus::LoadScriptFromFile(reader.GetFilePath())) {
        std::print(std::cout, "syntax error: {}\n", GetError(Varjus::GetErrorMessage()));
        return ExitApp(0);
    }

    if (const auto returnValue = Varjus::ExecuteScript()) {
        std::print(std::cout, "the program returned: {}\n", returnValue->ToPrintableString());
    } else {
        std::print(std::cout, "runtime error: {}\n", GetError(Varjus::GetErrorMessage()));
    }
    
    return ExitApp(1);

}