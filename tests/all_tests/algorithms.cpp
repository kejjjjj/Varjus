#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "algorithms"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Fibonacci(7)") {

	auto retVal = TEST_ExecuteFile(JP("fibonacci.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("IterativeFibonacci(7)") {

	auto retVal = TEST_ExecuteFile(JP("fibonacci_iterative.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Factorial(7)") {

	auto retVal = TEST_ExecuteFile(JP("factorial.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5040);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("FactorialIterative(7)") {

	auto retVal = TEST_ExecuteFile(JP("factorial_iterative.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5040);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}