#include "tests/tests.hpp"
#include "tests/catch_amalgamated.hpp"
#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

#define PATH_PREFIX "global_variables"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)


TEST_CASE("Main function references global variable") {

	auto retVal = TEST_ExecuteFile(JP("main_references_global.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 30);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main function edits global variable") {

	auto retVal = TEST_ExecuteFile(JP("main_edits_global.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Main function and global scope edit global variable") {

	auto retVal = TEST_ExecuteFile(JP("main_and_global_edit_global.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}