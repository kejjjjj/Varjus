#pragma once

#include <array>

class IValue;
class CVariable;

using OperatorFunc = IValue*(*)(IValue*, IValue*);

IValue* OP_ASSIGNMENT(IValue* lhs, IValue* rhs);
IValue* OP_ADDITION(IValue* lhs, IValue* rhs);
IValue* OP_SUBTRACTION(IValue* lhs, IValue* rhs);
IValue* OP_MULTIPLICATION(IValue* lhs, IValue* rhs);
IValue* OP_DIVISION(IValue* lhs, IValue* rhs);
IValue* OP_MODULO(IValue* lhs, IValue* rhs);

IValue* OP_LESS_THAN(IValue* lhs, IValue* rhs);
IValue* OP_LESS_EQUAL(IValue* lhs, IValue* rhs);

IValue* OP_GREATER_THAN(IValue* lhs, IValue* rhs);
IValue* OP_GREATER_EQUAL(IValue* lhs, IValue* rhs);

IValue* OP_EQUALITY(IValue* lhs, IValue* rhs);
IValue* OP_UNEQUALITY(IValue* lhs, IValue* rhs);

IValue* OP_STRICT_EQUALITY(IValue* lhs, IValue* rhs);
IValue* OP_STRICT_UNEQUALITY(IValue* lhs, IValue* rhs);

IValue* OP_LOGICAL_AND(IValue* lhs, IValue* rhs);
IValue* OP_LOGICAL_OR(IValue* lhs, IValue* rhs);

IValue* OP_LEFT_SHIFT(IValue* lhs, IValue* rhs);
IValue* OP_RIGHT_SHIFT(IValue* lhs, IValue* rhs);

IValue* OP_BITWISE_OR(IValue* lhs, IValue* rhs);
IValue* OP_BITWISE_XOR(IValue* lhs, IValue* rhs);
IValue* OP_BITWISE_AND(IValue* lhs, IValue* rhs);

IValue* OP_ASSIGNMENT_ADDITION(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_SUBTRACTION(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_MULTIPLICATION(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_DIVISION(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_MODULO(IValue* lhs, IValue* rhs);

IValue* OP_ASSIGNMENT_LEFT_SHIFT(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_RIGHT_SHIFT(IValue* lhs, IValue* rhs);

IValue* OP_ASSIGNMENT_BITWISE_OR(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_BITWISE_XOR(IValue* lhs, IValue* rhs);
IValue* OP_ASSIGNMENT_BITWISE_AND(IValue* lhs, IValue* rhs);

#define NUM_OPERATORS 32ull

//VERY IMPORTANT THAT THESE HAVE THE SAME ORDER AS CPunctuation
constexpr std::array<OperatorFunc, NUM_OPERATORS> m_oOperatorTable = { {

	nullptr,          // p_error
	OP_ADDITION,      // p_add
	OP_SUBTRACTION,   // p_sub
	OP_MULTIPLICATION,// p_multiplication,
	OP_DIVISION,	  // p_division,
	OP_MODULO,	      // p_modulo,

	OP_LESS_THAN,     // p_less_than
	OP_LESS_EQUAL,    // p_less_equal
	OP_GREATER_THAN,  // p_greater_than
	OP_GREATER_EQUAL, // p_greater_equal

	OP_EQUALITY,          // p_equality
	OP_UNEQUALITY,        // p_unequality
	OP_STRICT_EQUALITY,   // p_strict_equality
	OP_STRICT_UNEQUALITY, // p_strict_unequality

	OP_LOGICAL_AND,   // p_logical_and,
	OP_LOGICAL_OR,    // p_logical_or,

	OP_LEFT_SHIFT,    // p_left_shift
	OP_RIGHT_SHIFT,   // p_right_shift

	OP_BITWISE_OR,    // p_bitwise_or
	OP_BITWISE_XOR,   // p_bitwise_xor
	OP_BITWISE_AND,   // p_bitwise_and

	OP_ASSIGNMENT,    // p_assign

	OP_ASSIGNMENT_ADDITION,	     // p_assignment_addition
	OP_ASSIGNMENT_SUBTRACTION,	 // p_assignment_subtraction
	OP_ASSIGNMENT_MULTIPLICATION,// p_assignment_multiplication
	OP_ASSIGNMENT_DIVISION,	     // p_assignment_division
	OP_ASSIGNMENT_MODULO,	     // p_assignment_modulo

	OP_ASSIGNMENT_LEFT_SHIFT, 	// p_assignment_left_shift
	OP_ASSIGNMENT_RIGHT_SHIFT,	// p_assignment_right_shift

	OP_ASSIGNMENT_BITWISE_OR,	// p_assignment_bitwise_or
	OP_ASSIGNMENT_BITWISE_XOR,	// p_assignment_bitwise_xor
	OP_ASSIGNMENT_BITWISE_AND	// p_assignment_bitwise_and
} };


