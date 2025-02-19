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

        Varjus::Init();
        Varjus::UseStdLibrary();

        const auto reader = VarjusIOReader("\\scripts\\" + srcFile);

        std::cout << reader.GetFilePath() << '\n';

        auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(reader.GetFilePath());
        auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(begin, end, reader.GetFilePath());

        if (!linter.Parse())
            return nullptr;

        CProgramRuntime runtime(CModule::ToRuntimeModules());
        return runtime.Execute();
    }
    catch (std::exception& ex) {
        std::cout << "\n\nERROR:\n" << ex.what() << "\n\n";
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}
