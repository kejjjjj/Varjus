#include <iostream>


#include "fs/fs_io.hpp"

#include "linter/tokenizer.hpp"
#include "linter/error.hpp"

#include "linter/expressions/expression.hpp"

int Failure(const std::string_view& msg)
{
    std::cerr << msg << "\n";
    std::cin.get();
    return 1;
}

int main()
{
    
    const auto reader = VarjusIOReader("\\scripts\\script.var");

    const auto fileBuf = reader.IO_Read();

    if (!fileBuf) {
        return Failure("!fileBuf");
    }

    auto tokenizer = CBufferTokenizer(*fileBuf);

    if(!tokenizer.Tokenize())
        return Failure("failed during tokenization");

    try {

		auto tokens = tokenizer.GetResult();

		auto begin = tokens.begin();
        auto end = tokens.end();

		CLinterExpression expr(begin, end);
		if (expr.ParseExpression())
			std::cout << "Success\n";
		else
			std::cout << "Failure\n";

    }
	catch (std::exception& e) {
		std::cerr << 
            "\n-----------LINTING ERROR-----------\n\n" 
            << e.what() << "\n" <<
            "-----------------------------------";
	}


    std::cin.get();

}