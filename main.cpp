#include <iostream>

#include "api/varjus_api.hpp"

#include "linter/tokenizer.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/context.hpp"
#include "linter/modules/module.hpp"
#include "runtime/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "runtime/exceptions/exception.hpp"

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"

int main()
{

    try {

        Varjus::Init();
        Varjus::UseStdLibrary();


        const auto reader = VarjusIOReader("scripts\\script.var");

        auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(reader.GetFilePath());
        auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
		auto begin = tokens.begin();
        auto end = tokens.end();

		CBufferLinter linter(begin, end, reader.GetFilePath());

        if (!linter.Parse())
            throw std::exception("couldn't parse the input file");

        CProgramRuntime runtime(CModule::ToRuntimeModules());
        runtime.Execute();

    }
    catch (CLinterError& e) {
        std::cerr << "\033[31m" <<
            "\n-----------LINTER ERROR-----------\n\n"
            << e.what() <<
            "\n\n--------------------------------" << "\033[0m\n";
    }
    catch (CRuntimeError& e) {
		std::cerr << "\033[31m" <<
            "\n--------------ERROR--------------\n\n" 
            << e.what() <<
            "\n\n-------------------------------" << "\033[0m\n";
	}

	std::cout << "Press ENTER to exit\n";
    std::cin.get();

    return 0;
}