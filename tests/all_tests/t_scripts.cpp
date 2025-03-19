#include "tests/tests.hpp"
#include "tests/utils.hpp"

TEST_CASE("hello, world as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn main() { return 'Hello, world!'; }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_string);

	REQUIRE(retVal->AsString() == VSL("Hello, world!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("1 as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn main() { return 1; }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_int);

	REQUIRE(retVal->AsInt() == 1);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("1u as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn main() { return 1u; }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_uint);

	REQUIRE(retVal->AsUInt() == 1u);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("1.0 as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn main() { return 1.0; }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_double);

	REQUIRE(retVal->AsDouble() == 1.0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("add(2, 3) as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn add(a, b) { return a + b; } fn main() { return add(2, 3u); }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_int);

	REQUIRE(retVal->AsInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE("add(2, 3) lambda as script") {

	auto retVal = TEST_ExecuteScript(VSL("fn main() { return ((a,b) => a + b)(2, 3u); }"));

	REQUIRE(retVal);
	REQUIRE(retVal->Type() == t_int);

	REQUIRE(retVal->AsInt() == 5);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}