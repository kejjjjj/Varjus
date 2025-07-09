#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("shift")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("integer shift operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("int.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		2 << 16,
		16 >> 2,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("uinteger shift operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("uint.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		VarjusUInt(2) << 30,
		VarjusUInt(30u) >> 2,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}