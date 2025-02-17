#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "operators\\bitwise"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)


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
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
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
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}