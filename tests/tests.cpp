#pragma once
#include "tests.hpp"

#include "fs/fs_io.hpp"

#include "linter/tokenizer.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"

#include "api/varjus_api.hpp"

#include "runtime/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

IValue* TEST_ExecuteFile(const std::string& srcFile)
{
    try {
        Varjus::Cleanup();
        Varjus::UseStdLibrary();

        const auto reader = VarjusIOReader("\\scripts\\" + srcFile);
        std::cout << reader.GetFilePath() << '\n';

        if (!Varjus::LoadScriptFromFile(reader.GetFilePath())) {
            return nullptr;
        }
        return Varjus::ExecuteScript();
    }
    catch (std::exception& ex) {
        std::cout << "\n\nERROR:\n" << ex.what() << "\n\n";
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}
