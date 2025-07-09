#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("std")) + VARJUS_DIRECTORY_SEPARATOR + VSL("regex")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("regexsearch()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("search.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true, false
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("regexfullmatch()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("full_match.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true, false
		}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("regexreplace()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("replace.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("The rxxx in Spxxx stays mxxxly in the plxxx."));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
