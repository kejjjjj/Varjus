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

TEST_CASE("ReverseString(hello)") {

	auto retVal = TEST_ExecuteFile(JP("reverse_string.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "olleh");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Counter(Hello, World!)") {

	auto retVal = TEST_ExecuteFile(JP("counter.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 13, 11 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("MaxSubArray([-2, 1, -3, 4, -1, 2, 1, -5, 4])") {

	auto retVal = TEST_ExecuteFile(JP("max_sub_array.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 6);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("GenerateParenthesis(3)") {

	auto retVal = TEST_ExecuteFile(JP("generate_parenthesis.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { "((()))","(()())","(())()","()(())","()()()" }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("RemoveDuplicates([1, 1, 2, 2, 3, 3])") {

	auto retVal = TEST_ExecuteFile(JP("remove_duplicates.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 2, 3 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("BinarySearch([1, 2, 3, 4, 5, 6])") {

	auto retVal = TEST_ExecuteFile(JP("binary_search.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 3);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("SumArray([10, 15, 20])") {

	auto retVal = TEST_ExecuteFile(JP("sum_array.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 45);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}