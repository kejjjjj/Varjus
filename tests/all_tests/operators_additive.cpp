#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + DIRECTORY_SEPARATOR + VSL("additive")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("integer additive operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("int.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		1 + 2,
		5 + 2,
		1001 + 50,
		-200 + 5,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("uinteger additive operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("uint.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		1 + 2,
		5 + 2,
		1001 + 50,
		static_cast<VarjusUInt>(-1),
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("double additive operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("double.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_DOUBLE>(t_double, { 
		1.2 + 2.6,
		5.1 + 2.74,
		1001.1234 + 50.1234,
		-200.2 + 5,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("string additive operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("string.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("hello world"),
		VSL("\"h\"\"e\""),
		VSL("hello\r\n\t\tworld"),
		VSL("\'hello, world\'"),
		VSL("\"hello, world\"")
		}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}