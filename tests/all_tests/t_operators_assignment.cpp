#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("assignment")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("assignment operators")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("script.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		10 + 10,
		10 - 5,
		10 * 2,
		10 / 2,
		10 % 2,
		10 << 2,
		10 >> 2,
		10 | 2,
		10 ^ static_cast<int>(2),
		10 & 2
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("swap operators")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("swap.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		2, 1, 1, 0
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}