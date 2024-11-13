#pragma once

#include <string_view>
#include <array>

enum OperatorPriority : char
{
	op_failure,
	op_assignment,		//	= += -= *= /= %= >>= <<= &= ^= |=
	op_conditional,		//	?:
	op_logical_or,		//	||
	op_logical_and,		//	&&
	op_bitwise_or,		//  | 
	op_bitwise_xor,		//	^
	op_bitwise_and,		//  & 
	op_equality,		//	< <= > >=
	op_relational,		//	== !=
	op_shift,			//  <<>>
	op_additive,		//	+-
	op_multiplicative,	//	* / %
	op_unary,			//  + - ! ~ ++ - - (type)* & sizeof	
	op_postfix,			//  () [] -> . ++ --	

};
enum Punctuation : char
{
	p_error,

	p_add,
	p_sub,

	p_less_than,
	p_greater_than,

	p_assign,

	p_equality,
	p_unequality,
	p_modulo,
	p_multiplication,
	p_division,

	p_logical_and,

	p_increment,

	p_par_open,
	p_par_close,

	p_bracket_open,
	p_bracket_close,

	p_curlybracket_open,
	p_curlybracket_close,

	p_comma,
	p_period,
	p_semicolon,
	p_colon
};

struct CPunctuation final
{
	std::string_view m_sIdentifier;
	Punctuation m_ePunctuation{};
	OperatorPriority m_ePriority{};
};

constexpr std::array<CPunctuation, 22u> punctuations
{
	CPunctuation{"==", p_equality, op_relational},
	CPunctuation{"!=", p_unequality, op_relational},

	CPunctuation{"&&", p_logical_and, op_logical_and},

	CPunctuation{"++", p_increment, op_unary},

	CPunctuation{"+", p_add, op_additive},
	CPunctuation{"-", p_sub, op_additive},

	CPunctuation{"<", p_less_than, op_equality},
	CPunctuation{">", p_greater_than, op_equality},

	CPunctuation{"%", p_modulo, op_multiplicative},
	CPunctuation{"*", p_multiplication, op_multiplicative},
	CPunctuation{"/", p_division, op_multiplicative},

	CPunctuation{"=", p_assign, op_assignment},

	CPunctuation{"(", p_par_open, op_postfix},
	CPunctuation{")", p_par_close, op_failure},

	CPunctuation{"[", p_bracket_open, op_postfix},
	CPunctuation{"]", p_bracket_close, op_failure},

	CPunctuation{"{", p_curlybracket_open, op_failure},
	CPunctuation{"}", p_curlybracket_close, op_failure},

	CPunctuation{",", p_comma},
	CPunctuation{".", p_period, op_postfix},
	CPunctuation{";", p_semicolon },
	CPunctuation{":", p_colon}

};