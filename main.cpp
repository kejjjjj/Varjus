#include <iostream>


#include "fs/fs_io.hpp"

#include "linter/tokenizer.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/expressions/expression.hpp"

#include "runtime/runtime.hpp"

int Failure(const std::string_view& msg)
{
    std::cerr << msg << "\n";
    std::cin.get();
    return 1;
}

int main()
{
    try {
        const auto reader = VarjusIOReader("\\scripts\\script.var");
        const auto fileBuf = reader.IO_Read();

        if (!fileBuf) {
            throw std::exception("couldn't read the file buffer");
        }

        auto tokenizer = CBufferTokenizer(*fileBuf);

        if (!tokenizer.Tokenize())
            throw std::exception("tokenization failure");

		auto tokens = tokenizer.GetTokens();
		auto begin = tokens.begin();
        auto end = tokens.end();

		CFileLinter linter(begin, end);

        if (!linter.ParseFile())
            throw std::exception("couldn't parse the input file");


        CProgramRuntime runtime(linter.GetRuntimeInformation());
        runtime.Execute();

     
    }
	catch (std::exception& e) {
		std::cerr << "\033[31m" <<
            "\n--------------ERROR--------------\n\n" 
            << e.what() <<
            "\n\n-------------------------------" << "\033[0m\n";
	}

	std::cout << "Press ENTER to exit\n";
    std::cin.get();
}


