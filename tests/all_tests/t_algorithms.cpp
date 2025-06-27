#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("algorithms")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Fibonacci(7)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("fibonacci.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("IterativeFibonacci(7)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("fibonacci_iterative.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Factorial(7)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("factorial.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5040);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("FactorialIterative(7)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("factorial_iterative.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 5040);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("ReverseString(hello)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("reverse_string.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("olleh"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Counter(Hello, World!)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("counter.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 13, 11 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("MaxSubArray([-2, 1, -3, 4, -1, 2, 1, -5, 4])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("max_sub_array.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 6);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("GenerateParenthesis(3)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("generate_parenthesis.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("((()))"),VSL("(()())"),VSL("(())()"),VSL("()(())"),VSL("()()()") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("RemoveDuplicates([1, 1, 2, 2, 3, 3])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("remove_duplicates.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 2, 3 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("BinarySearch([1, 2, 3, 4, 5, 6])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("binary_search.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 3);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("SumArray([10, 15, 20])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("sum_array.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 45);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("IsPrime() with 29 and 12")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("is_prime.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("isPalindrome() with 29 and 12")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("is_palindrome.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("MergeSort([38, 27, 43, 3, 9, 82, 10])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("merge_sort.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 3, 9, 10, 27, 38, 43, 82 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("KMPStringMatch(ababcababcabc, abc)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("kmp_string_match.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("ArrayToString([ 1, [ 2, [3, [] ] ], 4 ])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_to_string.var")));

	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("123empty4"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("StressTest")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("stress_test.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 93, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("FractalGenerator(10)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("fractalgenerator.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 41148);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("ExpressionEvaluator()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("expression_evaluator.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("17.0"), VSL("3.5"), VSL("64.0")}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("AutoComplete()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("auto_complete.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { 
		VSL("ape"), VSL("app"), VSL("apple"),
		VSL("bat"), VSL("bath"), VSL("banana") 
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}