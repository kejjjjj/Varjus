#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX std::string("operators") + DIRECTORY_SEPARATOR + "assignment"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("assignment operators") {

	auto retVal = TEST_ExecuteFile(JP("script.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		10 + 10,
		10 - 5,
		10 * 2,
		10 / 2,
		10 % 2,
		10 << 2,
		10 >> 2,
		10 | 2,
		10 ^ 2,
		10 & 2
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}