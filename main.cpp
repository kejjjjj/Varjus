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

DEFINE_CALLBACK(CppFunc, args) {
    return CStringValue::Construct("Hello from C++: " + args[0]->ValueAsString());
}

int main()
{
    Varjus::AddNewCallback("cppFunc", CppFunc, 1);

    Varjus::UseStdLibrary();


    const auto reader = VarjusIOReader("/scripts/script.var");
    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!Varjus::LoadScriptFromFile(reader.GetFilePath())) {
        std::cout << "syntax error: " << GetError(Varjus::GetErrorMessage()) << '\n';
        return ExitApp(0);
    }

    if (const auto returnValue = Varjus::ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << '\n';
    }
    else {
        std::cout << "runtime error: " << GetError(Varjus::GetErrorMessage()) << '\n';
    }
    
    return ExitApp(1);

}

