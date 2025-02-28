#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "statements"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("if statement is true v === 50") {

	auto retVal = TEST_ExecuteFile(JP("if_statement_is_true.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("else if statement is true v === 50") {

	auto retVal = TEST_ExecuteFile(JP("else_if_statement_is_true.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("else statement is true") {

	auto retVal = TEST_ExecuteFile(JP("else_statement.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("for loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_is_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("for loop iterates to 5 but breaks at 2") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_is_5_breaks_at_2.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("for loop iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("for_statement_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("while loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("while_less_than_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("while loop iterates to 5 but breaks at 2") {

	auto retVal = TEST_ExecuteFile(JP("while_less_than_5_breaks_at_2.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("while loop iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("while_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("repeat loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("repeat_until_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("repeat-while loop iterates to 5") {

	auto retVal = TEST_ExecuteFile(JP("repeat_while_less_than_5.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("repeat iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("repeat_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("repeat-while iterates to 5 and continues when even") {

	auto retVal = TEST_ExecuteFile(JP("repeat_while_odd_numbers.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,3,5 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("ranged for loop iterates [1, 2, 3]") {

	auto retVal = TEST_ExecuteFile(JP("ranged_for_temp_array.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,2,3 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("ranged for loop iterates variable [1, 2, 3]") {

	auto retVal = TEST_ExecuteFile(JP("ranged_for_array.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{t_int, { 1,2,3 }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("ranged for loop iterates \"hello\"") {

	auto retVal = TEST_ExecuteFile(JP("ranged_for_temp_string.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>{t_string, { "h", "e", "l", "l", "o"}});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("ranged for loop iterates variable \"hello\"") {

	auto retVal = TEST_ExecuteFile(JP("ranged_for_string.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>{t_string, { "h", "e", "l", "l", "o" }});

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}