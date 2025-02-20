#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "fmt_strings"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("`hello, world!`") {

	auto retVal = TEST_ExecuteFile(JP("no_formatting.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "hello, world!");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("``") {

	auto retVal = TEST_ExecuteFile(JP("empty_string.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`${0xffff}`") {

	auto retVal = TEST_ExecuteFile(JP("single_expression.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "65535");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`before ${0xffff} after`") {

	auto retVal = TEST_ExecuteFile(JP("text_expression_text.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "before 65535 after");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`${0xffff} and ${0.2}`") {

	auto retVal = TEST_ExecuteFile(JP("expression_text_expression.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "65535 and 0.200000");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`thing(): ${thing(20, 4)}`") {

	auto retVal = TEST_ExecuteFile(JP("function_call.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "thing(): 80");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`A${a}B${b}C${c}`") {

	auto retVal = TEST_ExecuteFile(JP("mixed.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "A0B1C2");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`o\nA${a}\nB${b}\nC${c}\no`") {

	auto retVal = TEST_ExecuteFile(JP("newline.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "o\n        A0\n        B1\n        C2\n    o");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("`${[(f(2,3))][0]}`") {

	auto retVal = TEST_ExecuteFile(JP("misc.var"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == "5");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}