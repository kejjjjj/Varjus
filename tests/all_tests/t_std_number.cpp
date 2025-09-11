#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("std")) + VARJUS_DIRECTORY_SEPARATOR + VSL("number")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("stoi()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("stoi.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		2, 
		-2,
		255
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("stou()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("stou.var")));
	
	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		2u,
		static_cast<VarjusUInt>(-1),
		255u
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("stod()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("stod.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_DOUBLE>(t_double, {
		2,
		-2.22,
		2.22,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("setprecision()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("setprecision.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("2.22"),
		VSL("2.2222"),
		VSL("2"),
		VSL("2.22220")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}