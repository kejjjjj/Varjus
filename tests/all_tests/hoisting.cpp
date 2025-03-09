#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VSL("hoisting")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Main function calls a function declared beneath it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("hoisted_single_file_function.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}