#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "unary"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("Unary typeof") {

	auto retVal = TEST_ExecuteFile(JP("typeof.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"undefined",
		"boolean",
		"int",
		"uint",
		"double",
		"string",
		"callable",
		"callable",
		"array",
		"object"
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Unary tostring") {

	auto retVal = TEST_ExecuteFile(JP("tostring.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"true",
		"[ 1, 2 ]",
		"{\n    a: hello,\n    b: world\n}",
		"2.400000",
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}