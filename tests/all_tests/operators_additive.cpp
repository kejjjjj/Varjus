#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "operators\\additive"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("integer additive operations") {

	auto retVal = TEST_ExecuteFile(JP("int.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		1 + 2,
		5 + 2,
		1001 + 50,
		-200 + 5,
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("uinteger additive operations") {

	auto retVal = TEST_ExecuteFile(JP("uint.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_UINT>(t_uint, {
		1 + 2,
		5 + 2,
		1001 + 50,
		0xFFFFFFFFFFFFFFFF,
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("double additive operations") {

	auto retVal = TEST_ExecuteFile(JP("double.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_DOUBLE>(t_double, { 
		1.2 + 2.6,
		5.1 + 2.74,
		1001.1234 + 50.1234,
		-200.2 + 5,
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("string additive operations") {

	auto retVal = TEST_ExecuteFile(JP("string.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"hello world",
		"\"h\"\"e\"",
		"hello\n		world",
		"\'hello, world\'",
		"\"hello, world\""
		}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}