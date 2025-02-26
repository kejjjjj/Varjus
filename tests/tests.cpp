#include "tests.hpp"

#include "fs/fs_io.hpp"

#include "linter/tokenizer.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"

#include "api/varjus_api.hpp"

#include "api/internal/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

#include <iostream>

IValue* TEST_ExecuteFile(const std::string& srcFile)
{
    try {
        //silly
        static std::unique_ptr<Varjus::State> state;
        state = std::make_unique<Varjus::State>();

        if (!state->UseStdLibrary())
            return nullptr;

        const auto reader = VarjusIOReader(DIRECTORY_SEPARATOR + std::string("scripts") + DIRECTORY_SEPARATOR + srcFile);
        std::cout << reader.GetFilePath() << '\n';

        const auto GetError = [](const std::optional<std::string>& errorMsg) {
            return errorMsg ? *errorMsg : "unknown error!";
        };
    
        if (!state->LoadScriptFromFile(reader.GetFilePath())) {
            std::cout << "syntax error: " << GetError(state->GetErrorMessage()) << '\n';
            return nullptr;
        }
        return state->ExecuteScript();
    }
    catch (std::exception& ex) {
        std::cout << "\n\nERROR:\n" << ex.what() << "\n\n";
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}
