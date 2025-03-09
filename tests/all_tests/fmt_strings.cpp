#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("fmt_strings")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("`hello, world!`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("no_formatting.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("hello, world!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("``")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("empty_string.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL(""));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`${0xffff}`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("single_expression.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("65535"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`before ${0xffff} after`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("text_expression_text.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("before 65535 after"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`${0xffff} and ${0.2}`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("expression_text_expression.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("65535 and 0.200000"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`thing(): ${thing(20, 4)}`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("function_call.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("thing(): 80"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`A${a}B${b}C${c}`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("mixed.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("A0B1C2"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`o\nA${a}\nB${b}\nC${c}\no`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("newline.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("o\n        A0\n        B1\n        C2\n    o"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("`${[(f(2,3))][0]}`")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("misc.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("5"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}