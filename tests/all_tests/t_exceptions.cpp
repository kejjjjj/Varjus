#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("exceptions")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("Simple throw within try catch"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("simple_try_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("test exception"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);

}
TEST_CASE(("Nested try catch"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("nested_try_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("test exception"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Triple nested try catch"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("triple_nested_try_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("exceptionFunc"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Deeply nested try catch"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("deeply_nested_try_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("exceptionFunc"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("try catch doesn't throw"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("try_catch_no_throw.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("try"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("try catch returns from try block"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("try_catch_returns_from_try.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("try"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("try catch returns from catch block"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("try_catch_returns_from_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("catch"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("try catch returns the exception from catch block"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("try_catch_returns_exception_from_catch.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("catch"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("function throws and gets caught in main"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("throw_exception_from_function.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("catch"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("function throws and catches it"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("function_catches_exception.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("caught"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Assigning an exception"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("assign_an_exception.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("exception!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Assigning an exception2"))
{
	auto retVal = TEST_ExecuteFile(JP(VSL("assign_an_exception2.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("exception!2"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}