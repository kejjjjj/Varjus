#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("std")) + VARJUS_DIRECTORY_SEPARATOR + VSL("string")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE(("from_code()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("from_code.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("hello!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}