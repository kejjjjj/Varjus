#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VSL("hoisting")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("Main function calls a function declared beneath it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("hoisted_single_file_function.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Main function calls two functions declared beneath it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("two_hoisted_functions.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 99);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}