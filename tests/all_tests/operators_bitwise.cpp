#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX std::string("operators") + DIRECTORY_SEPARATOR + "bitwise"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)


TEST_CASE("integer bitwise operations") {

	auto retVal = TEST_ExecuteFile(JP("int.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		16 & 4,
		20 & 4,

		16 | 32,
		512 | 5,

		12 ^ 6,
		5 ^ 2,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("uinteger bitwise operations") {

	auto retVal = TEST_ExecuteFile(JP("uint.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_uint, {
		16 & 4,
		20 & 4,

		16 | 32,
		512 | 5,

		12 ^ 6,
		5 ^ 2,
		}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}