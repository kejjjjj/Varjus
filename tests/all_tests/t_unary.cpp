#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VSL("unary")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Unary negation")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("negation.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { 
		VSL("-5"),
		VSL("5"),
		VSL("-5.500000"), 
		VSL("5.500000")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Unary typeof")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("typeof.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("undefined"),
		VSL("boolean"),
		VSL("int"),
		VSL("uint"),
		VSL("double"),
		VSL("string"),
		VSL("callable"),
		VSL("callable"),
		VSL("array"),
		VSL("object")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Unary tostring")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("tostring.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("true"),
		VSL("[ 1, 2 ]"),
		VSL("{\n    a: hello,\n    b: world\n}"),
		VSL("2.400000"),
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}