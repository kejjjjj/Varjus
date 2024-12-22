#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "statements"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("if statement is true v === 50") {

	auto retVal = TEST_ExecuteFile(JP("if_statement_is_true.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("else if statement is true v === 50") {

	auto retVal = TEST_ExecuteFile(JP("else_if_statement_is_true.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("else statement is true") {

	auto retVal = TEST_ExecuteFile(JP("else_statement.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("for loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_is_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("for loop iterates to 5 but breaks at 2") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_is_5_breaks_at_2.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("for loop iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("while loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("while_less_than_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("while loop iterates to 5 but breaks at 2") {

	auto retVal = TEST_ExecuteFile(JP("while_less_than_5_breaks_at_2.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("while loop iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("while_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}