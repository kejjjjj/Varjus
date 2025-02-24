#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "coercion"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("Boolean gets coerced to int") {

	auto retVal = TEST_ExecuteFile(JP("boolean_to_int.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("Boolean gets coerced to uint") {

	auto retVal = TEST_ExecuteFile(JP("boolean_to_uint.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("Boolean gets coerced to double") {

	auto retVal = TEST_ExecuteFile(JP("boolean_to_double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 3.0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("UInt gets coerced to int") {

	auto retVal = TEST_ExecuteFile(JP("uint_to_int.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("Int gets coerced to double") {

	auto retVal = TEST_ExecuteFile(JP("int_to_double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 3.0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("UInt gets coerced to double") {

	auto retVal = TEST_ExecuteFile(JP("uint_to_double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 3.0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
