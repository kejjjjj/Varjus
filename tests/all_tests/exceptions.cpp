#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "exceptions"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Simple throw within try catch")
{
	auto retVal = TEST_ExecuteFile(JP("simple_try_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "test exception");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
