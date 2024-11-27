
#pragma warning(push, 0)
#include <boost/test/unit_test.hpp>
#pragma warning(pop)

#include "linter/tokenizer.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
BOOST_AUTO_TEST_CASE(TokenizerProducesCorrectTokens)
{

    // Arrange: Create tokenizer and input string
    CBufferTokenizer tokenizer("+ ++hello1 fn 1 2.0 return ;;;let");

    // Act: Tokenize the input
    tokenizer.Tokenize();
    const auto tokens = tokenizer.GetTokens();

    // Assert: Validate the total number of tokens
    BOOST_REQUIRE_EQUAL(tokens.size(), 11);

    // Assert: Validate token types
    std::vector<TokenType> expectedTypes = {
        tt_operator, tt_operator, tt_name, tt_fn, tt_int,
        tt_double, tt_return, tt_operator, tt_operator,
        tt_operator, tt_declaration
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        BOOST_REQUIRE_EQUAL(tokens[i]->Type(), expectedTypes[i]);
    }

}
