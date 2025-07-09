#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("expressions")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE(("operator precedence tests in simple expressions"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("operator_precedence.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		2 + 3 * 2,
		(2 + 3) * 2,
		1 * (7 > 2 * 3),
		((1 << 16) + 2) | 257,
		(((4 - 2) * 6) << 5) % 5
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("operator precedence tests in sequences"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("sequence.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		(static_cast<void>(2 + 3), static_cast<void>(2), 4),
		(static_cast<void>(2 + 3), 4),
		2 + (static_cast<void>(2 + 3), 4),
		2 + (static_cast<void>(2 + 3), 4) + 3
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("operator precedence in ternary operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("ternary.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("f"), VSL("f2"), VSL("f"), VSL("t2") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("conditional and shortcuts"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("conditional_and.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		false,
		true,
		true
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("conditional or shortcuts"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("conditional_or.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true,
		true,
		false
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}