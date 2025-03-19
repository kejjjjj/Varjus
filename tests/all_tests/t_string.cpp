#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("string")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)


TEST_CASE("hello.toupper") {

	auto retVal = TEST_ExecuteFile(JP(VSL("toupper.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("HELLO!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("hello.tolower") {

	auto retVal = TEST_ExecuteFile(JP(VSL("tolower.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("hello!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("hello.substring") {

	auto retVal = TEST_ExecuteFile(JP(VSL("substring.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("el"), VSL("llo")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("string.split") {

	auto retVal = TEST_ExecuteFile(JP(VSL("split.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("This"), VSL("is"), VSL("some"), VSL("example"), VSL("text")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("string.replace") {

	auto retVal = TEST_ExecuteFile(JP(VSL("replace.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("Thwas was some example text... and thwas was some more"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("hello.clone(3)") {

	auto retVal = TEST_ExecuteFile(JP(VSL("clone.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("hello!hello!hello!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("HELLO.get_code_at(0)") {

	auto retVal = TEST_ExecuteFile(JP(VSL("get_code_at.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_int);

	REQUIRE(retVal->AsInt() == 72);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("hello_world.contains") {

	auto retVal = TEST_ExecuteFile(JP(VSL("contains.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true, true, false
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}