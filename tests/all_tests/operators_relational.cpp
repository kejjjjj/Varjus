#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX std::string("operators") + DIRECTORY_SEPARATOR + "relational"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("boolean relational operations") {

	auto retVal = TEST_ExecuteFile(JP("boolean.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		false < true,
		false <= true,
		false > true,
		false >= true,

		true < false,
		true <= false,
		true > false,
		true >= false
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("integer relational operations") {

	auto retVal = TEST_ExecuteFile(JP("int.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		100 < 101,
		101 < 100,
		100 < 100,

		100 <= 101,
		101 <= 100,
		100 <= 100,

		100 > 101,
		101 > 100,
		100 > 100,

		100 >= 101,
		101 >= 100,
		100 >= 100,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("uinteger relational operations") {

	auto retVal = TEST_ExecuteFile(JP("uint.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		100 < 101,
		101 < 100,
		100 < 100,

		100 <= 101,
		101 <= 100,
		100 <= 100,

		100 > 101,
		101 > 100,
		100 > 100,

		100 >= 101,
		101 >= 100,
		100 >= 100,
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("double relational operations") {

	auto retVal = TEST_ExecuteFile(JP("double.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		100.1 < 101.1,
		101.1 < 100.1,
		100.1 < 100.1,

		100.1 <= 101.1,
		101.1 <= 100.1,
		100.1 <= 100.1,

		100.1 > 101.1,
		101.1 > 100.1,
		100.1 > 100.1,

		100.1 >= 101.1,
		101.1 >= 100.1,
		100.1 >= 100.1
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}