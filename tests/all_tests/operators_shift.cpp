#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "operators\\shift"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("integer shift operations") {

	auto retVal = TEST_ExecuteFile(JP("int.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		2 << 16,
		16 >> 2,
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("uinteger shift operations") {

	auto retVal = TEST_ExecuteFile(JP("uint.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		VarjusUInt(2) << 30,
		VarjusUInt(30u) >> 2,
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}