#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "functions"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Main function calls a function with the value 5 and returns it") {

	auto retVal = TEST_ExecuteFile(JP("function_returns_argument.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main function calls a function with the value [50, 1] and returns it") {

	auto retVal = TEST_ExecuteFile(JP("function_returns_shared_argument.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 50, 1 }});

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main function calls a function that edits a global variable to 50") {

	auto retVal = TEST_ExecuteFile(JP("function_edits_global_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main function calls a function that doesn't return") {

	auto retVal = TEST_ExecuteFile(JP("function_no_return.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_undefined);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main function calls a function that recursively calls itself until v >= 5") {

	auto retVal = TEST_ExecuteFile(JP("simple_recursion.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}