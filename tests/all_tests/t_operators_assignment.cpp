#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + DIRECTORY_SEPARATOR + VSL("assignment")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

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