#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("std")) + VARJUS_DIRECTORY_SEPARATOR + VSL("json")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("parse()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("parse.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("7"),
		VSL("3"),
		VSL("JSON"),
		VSL("Wonderland")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("parse_fail()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("parse_fail.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("fail!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("stringify()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("stringify.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);

	constexpr auto r = VSL("{\"name\":\"Alice\",\"skills\":[ \"C++\", \"Python\", \"JSON\" ],\"address\":{\"street\":\"123 Main St\"}}");

	REQUIRE(retVal->ToString() == r);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
