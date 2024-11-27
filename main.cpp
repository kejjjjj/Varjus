#include <iostream>


#include "linter/tokenizer.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/context.hpp"

#include "runtime/runtime.hpp"

#include "fs/fs_io.hpp"

#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

int _main();

struct AllocatorSetup {
    AllocatorSetup() {  }
    ~AllocatorSetup() { _main(); }
};

BOOST_GLOBAL_FIXTURE(AllocatorSetup);

int _main()
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

        auto context = CProgramContext{};
        context.m_oAllMembers["length"];

		CFileLinter linter(begin, end, &context);

        if (!linter.ParseFile())
            throw std::exception("couldn't parse the input file");


        CProgramRuntime runtime(linter.GetRuntimeInformation(), &context);
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


