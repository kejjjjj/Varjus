
#ifndef _DEBUG

#pragma warning(push, 0)
#include <boost/test/unit_test.hpp>

#pragma warning(pop)

#include "linter/functions/function.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/tokenizer.hpp"
#include "linter/linter.hpp"
#include "linter/context.hpp"
#include "tests/test.hpp"

BOOST_AUTO_TEST_CASE(FunctionHasTwoInstructionsAndOneNested)
{
	auto string = 
	"fn main() {"
		"let f = 5;"
		"if(f < 6) {"
			"f = 6;"
		"}"
	"}";

	CBufferTokenizer t(string);
	BOOST_REQUIRE_EQUAL(t.Tokenize(), success);

	auto tokens = t.GetTokens();

	auto begin = tokens.begin();
	auto end = tokens.end();

	auto context = CProgramContext{};
	context.m_oAllMembers["length"];

	CFileLinter linter(begin, end, &context);
	BOOST_REQUIRE_EQUAL(linter.ParseFile(), success);

	auto info = linter.GetRuntimeInformation();
	auto main = info->FindFunction("main");
	BOOST_REQUIRE_NE(main, nullptr);

	BOOST_REQUIRE_EQUAL(main->NumInstructions(), 2);

	auto& if_instruction = main->GetInstruction(1);
	BOOST_REQUIRE_EQUAL(if_instruction->Type(), EStructureType::st_conditional);

	auto if_ = dynamic_cast<CRuntimeConditionalStatement*>(if_instruction.get());

	BOOST_REQUIRE_EQUAL(if_->NumInstructions(), 1);
}

#endif