#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "closures"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Closure captures local variable") {

	auto retVal = TEST_ExecuteFile(JP("closure_captures_local_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Closure captures global variable") {

	auto retVal = TEST_ExecuteFile(JP("closure_captures_global_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Closure gets assigned to a variable") {

	auto retVal = TEST_ExecuteFile(JP("closure_in_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Closure edits local variable") {

	auto retVal = TEST_ExecuteFile(JP("closure_edits_local_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Closure used in a closure") {

	auto retVal = TEST_ExecuteFile(JP("closure_used_in_other_closure.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Closure used in a closure (variable context)") {

	auto retVal = TEST_ExecuteFile(JP("closure_used_in_other_closure_variable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}