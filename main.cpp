#include <iostream>

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"

#include "varjus_api/varjus_api.hpp"
#include <thread>
#include <array>


int ExitApp(int v)
{
    std::cout << "Press ENTER to exit\n";
    std::cin.get();
    return v;
}

DEFINE_CALLBACK(CppFunc, args) {
    return CStringValue::Construct(ctx->m_pRuntime, "Hello from C++: " + args[0]->ValueAsString());
}


void PerThread(std::size_t i)
{
    Varjus::State state;


    const auto reader = VarjusIOReader(DIRECTORY_SEPARATOR_CHAR + "scripts"s + DIRECTORY_SEPARATOR_CHAR + "script.var"s);

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!state.UseStdLibrary() || !state.AddNewCallback("cppFunc", CppFunc, 1)) {
        std::cout << "state error: " << GetError(state.GetErrorMessage()) << " in " << i << '\n';
        return;
    }

    if (!state.LoadScriptFromFile(reader.GetFilePath())) {
        std::cout << "syntax error: " << GetError(state.GetErrorMessage()) << " in " << i << '\n';
        return;
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << " in " << i << '\n';
    }
    else {
        std::cout << "runtime error: " << GetError(state.GetErrorMessage()) << " in " << i << '\n';
    }
}

int main()
{
    
    std::array<std::thread, 16> threads;

    for (std::size_t i{}; auto& thread : threads)
        thread = std::thread(PerThread, i++);
    
    for (auto& thread : threads)
        thread.join();

    return ExitApp(1);
}

