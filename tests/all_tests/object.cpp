#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "object"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Object.set") {

	auto retVal = TEST_ExecuteFile(JP("set.var"));

	AssertObject(retVal, AssertObjectValue<ASSERT_STRING>(t_string, { 
		{"key0", "value0" },
		{"key1", "value1" },
		{"key2", "value2_edited" },
		{"key3", "value3" }
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Object.remove") {

	auto retVal = TEST_ExecuteFile(JP("remove.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"true", "false", "false", "2", "key1value1", "key2value2"
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Object.keys") {

	auto retVal = TEST_ExecuteFile(JP("keys.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"key0",
		"key1",
		"key2"
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Object.values") {

	auto retVal = TEST_ExecuteFile(JP("values.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"value0",
		"value1",
		"value2" 
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Object.contains") {

	auto retVal = TEST_ExecuteFile(JP("contains.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true,
		false,
		true
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Object.to_array") {

	auto retVal = TEST_ExecuteFile(JP("to_array.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		"key0value0", "key1value1", "key2value2"
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}