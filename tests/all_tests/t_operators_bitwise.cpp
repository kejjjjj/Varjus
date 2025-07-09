#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("bitwise")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE(("integer bitwise operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("int.var")));

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

TEST_CASE(("uinteger bitwise operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("uint.var")));

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