#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "hoisting"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Main function calls a function declared beneath it") {

	auto retVal = TEST_ExecuteFile(JP("hoisted_single_file_function.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}