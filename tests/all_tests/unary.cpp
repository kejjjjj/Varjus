#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "unary"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Unary typeof") {

	auto retVal = TEST_ExecuteFile(JP("typeof.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"undefined",
		"boolean",
		"int",
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