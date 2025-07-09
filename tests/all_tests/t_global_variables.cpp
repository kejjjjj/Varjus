#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("global_variables")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE(("Main function references global variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_references_global.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 30);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Main function edits global variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_edits_global.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Main function and global scope edit global variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_and_global_edit_global.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}