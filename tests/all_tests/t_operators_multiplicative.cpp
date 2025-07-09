#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("multiplicative")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("integer multiplicative operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("int.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		2 * 5,
		10 / 2,
		9 / 2,
		10 % 2,
		7 % 2 
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("uinteger multiplicative operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("uint.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		2 * 5,
		10 / 2,
		9 / 2,
		10 % 2,
		7 % 2
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("double multiplicative operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("double.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_DOUBLE>(t_double, { 
		2 * 5.2,
		2.5 * 3,
		2.2 * 2.3,

		10 / 2.5,
		9.2 / 2,
		11.2 / 4.2,

		std::fmod(10.3, 2),
		std::fmod(10, 2.5),
		std::fmod(7.1, 2.3) 

	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}