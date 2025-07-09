#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("closures")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("Closure captures local variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_captures_local_variable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Closure captures global variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_captures_global_variable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Closure gets assigned to a variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_in_variable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Closure edits local variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_edits_local_variable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Closure used in a closure")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_used_in_other_closure.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Closure used in a closure (variable context)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("closure_used_in_other_closure_variable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Self capturing recursive closure")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("self_capturing_closure.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 10);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}