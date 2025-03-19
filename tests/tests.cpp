#include "tests.hpp"

#include "fs/fs_io.hpp"

#include "linter/tokenizer.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/varjus_api.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

#include <iostream>

IValue* TEST_ExecuteFile(const VarjusString& srcFile)
{
    try {
        //silly
        static std::unique_ptr<Varjus::State> state;
        state = std::make_unique<Varjus::State>();

        if (!state->UseStdLibrary())
            return nullptr;

        const auto reader = VarjusIOReader(DIRECTORY_SEPARATOR + VarjusString(VSL("scripts")) + DIRECTORY_SEPARATOR + srcFile);
        fmt::print(STD_COUT, VSL("{}\n"), reader.GetFilePath());

        const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
            return errorMsg ? *errorMsg : VSL("unknown error!");
        };
    
        if (!state->LoadScriptFromFile(reader.GetFilePath())) {
            fmt::print(STD_COUT, VSL("syntax error: {}\n"), GetError(state->GetErrorMessage()));
            return nullptr;
        }
        return state->ExecuteScript();
    }
    catch (std::exception& ex) {
        fmt::print(STD_COUT, VSL("\n\nERROR:\n{}\n\n"), ex.what());
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}

IValue* TEST_ExecuteScript(const VarjusString& script)
{
    try {
        //silly
        static std::unique_ptr<Varjus::State> state;
        state = std::make_unique<Varjus::State>();

        if (!state->UseStdLibrary())
            return nullptr;
        fmt::print(STD_COUT, VSL("{}\n"), script);

        const auto GetError = [](const std::optional<VarjusString>& errorMsg) {
            return errorMsg ? *errorMsg : VSL("unknown error!");
            };

        if (!state->LoadScript(script)) {
            fmt::print(STD_COUT, VSL("syntax error: {}\n"), GetError(state->GetErrorMessage()));
            return nullptr;
        }
        return state->ExecuteScript();
    }
    catch (std::exception& ex) {
        fmt::print(STD_COUT, VSL("\n\nERROR:\n{}\n\n"), ex.what());
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}
