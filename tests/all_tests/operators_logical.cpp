#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "operators\\logical"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("logical and operations") {

	auto retVal = TEST_ExecuteFile(JP("logical_and.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { 
		true && true,
		true && false,
		false && true,

		false, //undefined && undefined

		1 && 1,
		0 && 0,
		1 && 0,

		1.0 && 1.0,
		0.0 && 0.0,
		1.0 && 0.0
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("logical or operations") {

	auto retVal = TEST_ExecuteFile(JP("logical_or.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true || true,
		true || false,
		false || true,

		false, //undefined || undefined

		1 || 1,
		0 || 0,
		1 || 0,

		1.0 || 1.0,
		0.0 || 0.0,
		1.0 || 0.0
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}