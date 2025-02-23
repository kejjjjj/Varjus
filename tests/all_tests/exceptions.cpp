#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "exceptions"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("Simple throw within try catch")
{
	auto retVal = TEST_ExecuteFile(JP("simple_try_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "test exception");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
TEST_CASE("Nested try catch")
{
	auto retVal = TEST_ExecuteFile(JP("nested_try_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "test exception");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Triple nested try catch")
{
	auto retVal = TEST_ExecuteFile(JP("triple_nested_try_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "exceptionFunc");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Deeply nested try catch")
{
	auto retVal = TEST_ExecuteFile(JP("deeply_nested_try_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "exceptionFunc");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("try catch doesn't throw")
{
	auto retVal = TEST_ExecuteFile(JP("try_catch_no_throw.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "try");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("try catch returns from try block")
{
	auto retVal = TEST_ExecuteFile(JP("try_catch_returns_from_try.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "try");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("try catch returns from catch block")
{
	auto retVal = TEST_ExecuteFile(JP("try_catch_returns_from_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "catch");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("try catch returns the exception from catch block")
{
	auto retVal = TEST_ExecuteFile(JP("try_catch_returns_exception_from_catch.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "catch");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("function throws and gets caught in main")
{
	auto retVal = TEST_ExecuteFile(JP("throw_exception_from_function.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "catch");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("function throws and catches it")
{
	auto retVal = TEST_ExecuteFile(JP("function_catches_exception.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "caught");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}