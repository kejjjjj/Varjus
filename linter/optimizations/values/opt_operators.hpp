#pragma once

#ifdef OPTIMIZATIONS

#include <array>

class IConstEvalValue;
using OptOperatorFunc = IConstEvalValue*(*)(IConstEvalValue*, IConstEvalValue*);

#define FORWARD_DECLARE_CONSTEXPR_OPERATOR(name)\
IConstEvalValue* name(IConstEvalValue* lhs, IConstEvalValue* rhs)

FORWARD_DECLARE_CONSTEXPR_OPERATOR(OPT_ASSIGN);
FORWARD_DECLARE_CONSTEXPR_OPERATOR(OPT_ADDITION);

#define NUM_OPERATORS 32ull

//VERY IMPORTANT THAT THESE HAVE THE SAME ORDER AS CPunctuation
constexpr std::array<OptOperatorFunc, NUM_OPERATORS> m_oOptOperatorTable = {{

	nullptr,          // p_error
	OPT_ADDITION,      // p_add
	nullptr,   // p_sub
	nullptr,// p_multiplication,
	nullptr,	  // p_division,
	nullptr,	      // p_modulo,

	nullptr,     // p_less_than
	nullptr,    // p_less_equal
	nullptr,  // p_greater_than
	nullptr, // p_greater_equal

	nullptr,          // p_equality
	nullptr,        // p_unequality
	nullptr,   // p_strict_equality
	nullptr, // p_strict_unequality

	nullptr,   // p_logical_and,
	nullptr,    // p_logical_or,

	nullptr,    // p_left_shift
	nullptr,   // p_right_shift

	nullptr,    // p_bitwise_or
	nullptr,   // p_bitwise_xor
	nullptr,   // p_bitwise_and

	OPT_ASSIGN,    // p_assign

	nullptr,	     // p_assignment_addition
	nullptr,	 // p_assignment_subtraction
	nullptr,// p_assignment_multiplication
	nullptr,	     // p_assignment_division
	nullptr,	     // p_assignment_modulo

	nullptr, 	// p_assignment_left_shift
	nullptr,	// p_assignment_right_shift

	nullptr,	// p_assignment_bitwise_or
	nullptr,	// p_assignment_bitwise_xor
	nullptr	// p_assignment_bitwise_and
} };


#endif
