#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("scope")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE("unnamed scope") {

	auto retVal = TEST_ExecuteFile(JP(VSL("unnamed_scope.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("scope!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("double unnamed scope") {

	auto retVal = TEST_ExecuteFile(JP(VSL("double_unnamed_scope.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("scope!2"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE("unnamed scope assignment") {

	auto retVal = TEST_ExecuteFile(JP(VSL("unnamed_scope_assignment.var")));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("scope!3"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}