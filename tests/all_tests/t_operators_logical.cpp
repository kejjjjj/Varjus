#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("logical")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("logical and operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("logical_and.var")));

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
	TEST_END(retVal);
}

TEST_CASE(("logical or operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("logical_or.var")));

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
	TEST_END(retVal);
}