#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "coercion"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Boolean gets coerced to int") {

	auto retVal = TEST_ExecuteFile(JP("boolean_to_int.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Boolean gets coerced to double") {

	auto retVal = TEST_ExecuteFile(JP("boolean_to_double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 3.0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Int gets coerced to double") {

	auto retVal = TEST_ExecuteFile(JP("int_to_double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 3.0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
