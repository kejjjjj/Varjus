#include <iostream>

#include "linter/tokenizer.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/context.hpp"
#include "linter/imports/module.hpp"
#include "runtime/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"
int main()
{

    try {

        const auto reader = VarjusIOReader("scripts\\script.var");
        auto fileBuf = reader.IO_Read();

        if (!fileBuf) {
            throw std::exception("couldn't read the file buffer");
        }

        fileBuf->push_back('\n'); // fixes a crash lol

        auto tokenizer = CBufferTokenizer(*fileBuf);

        if (!tokenizer.Tokenize())
            throw std::exception("the input file didn't have any parsable tokens");

		auto tokens = tokenizer.GetTokens();
		auto begin = tokens.begin();
        auto end = tokens.end();

        auto wd = fs::previous_directory(reader.GetFilePath());
		CFileLinter linter(begin, end, wd);

        if (!linter.ParseFile())
            throw std::exception("couldn't parse the input file");

        CProgramRuntime runtime(CModule::ToRuntimeModules());
        runtime.Execute();

     
    } catch (std::exception& e) {
		std::cerr << "\033[31m" <<
            "\n--------------ERROR--------------\n\n" 
            << e.what() <<
            "\n\n-------------------------------" << "\033[0m\n";
	}

	std::cout << "Press ENTER to exit\n";
    std::cin.get();

    return 0;
}