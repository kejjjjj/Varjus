#pragma once

#include <string_view>
#include <array>

enum OperatorPriority : char
{
	op_failure,
	op_assignment,		//	= += -= *= /= %= >>= <<= &= ^= |=
	op_conditional,		//	?
	op_conditional2,    //  :  
	op_logical_or,		//	||
	op_logical_and,		//	&&
	op_bitwise_or,		//  | 
	op_bitwise_xor,		//	^
	op_bitwise_and,		//  & 
	op_equality,		//	< <= > >=
	op_relational,		//	== != === !==
	op_shift,			//  <<>>
	op_additive,		//	+ -
	op_multiplicative,	//	* / %
	op_unary,			//  + - ! ~ ++ - - (type)* & sizeof	
	op_postfix,			//  () [] -> . ++ --	

};
enum Punctuation : char
{
	p_error,

	p_add,
	p_sub,
	p_multiplication,
	p_division,
	p_modulo,

	p_less_than,
	p_less_equal,
	p_greater_than,
	p_greater_equal,


	p_equality,
	p_unequality,
	p_strict_equality,
	p_strict_unequality,
	
	p_logical_and,
	p_logical_or,

	p_left_shift,
	p_right_shift,

	p_bitwise_or,
	p_bitwise_xor,
	p_bitwise_and,

	p_assign,
	p_assignment_addition,
	p_assignment_subtraction,
	p_assignment_multiplication,
	p_assignment_division,
	p_assignment_modulo,

	p_assignment_left_shift,
	p_assignment_right_shift,

	p_assignment_bitwise_or,
	p_assignment_bitwise_xor,
	p_assignment_bitwise_and,

	p_increment,
	p_decrement,

	p_par_open,
	p_par_close,

	p_bracket_open,
	p_bracket_close,

	p_curlybracket_open,
	p_curlybracket_close,

	p_comma,
	p_period,
	p_semicolon,
	p_colon,
	p_question_mark,
	p_exclamation,
	p_tilde,
};

struct CPunctuation final
{
	std::string_view m_sIdentifier;
	Punctuation m_ePunctuation{};
	OperatorPriority m_ePriority{};
};

constexpr std::array<CPunctuation, 46u> punctuations
{
	CPunctuation{"===", p_strict_equality, op_relational},
	CPunctuation{"!==", p_strict_unequality, op_relational},

	CPunctuation{"<<=", p_assignment_left_shift, op_assignment},
	CPunctuation{">>=", p_assignment_right_shift, op_assignment},

	CPunctuation{"+=", p_assignment_addition, op_assignment},
	CPunctuation{"-=", p_assignment_subtraction, op_assignment},
	CPunctuation{"*=", p_assignment_multiplication, op_assignment},
	CPunctuation{"/=", p_assignment_division, op_assignment},
	CPunctuation{"%=", p_assignment_modulo, op_assignment},
	CPunctuation{"|=", p_assignment_bitwise_or, op_assignment},
	CPunctuation{"^=", p_assignment_bitwise_xor, op_assignment},
	CPunctuation{"&=", p_assignment_bitwise_and, op_assignment},

	CPunctuation{"==", p_equality, op_relational},
	CPunctuation{"!=", p_unequality, op_relational},

	CPunctuation{"<=", p_less_equal, op_equality},
	CPunctuation{">=", p_greater_equal, op_equality},

	CPunctuation{"&&", p_logical_and, op_logical_and},
	CPunctuation{"||", p_logical_or, op_logical_or},

	CPunctuation{"<<", p_left_shift, op_shift},
	CPunctuation{">>", p_right_shift, op_shift},

	CPunctuation{"++", p_increment, op_unary},
	CPunctuation{"--", p_decrement, op_unary},
	CPunctuation{"~", p_tilde, op_unary},

	CPunctuation{"+", p_add, op_additive},
	CPunctuation{"-", p_sub, op_additive},

	CPunctuation{"<", p_less_than, op_equality},
	CPunctuation{">", p_greater_than, op_equality},

	CPunctuation{"%", p_modulo, op_multiplicative},
	CPunctuation{"*", p_multiplication, op_multiplicative},
	CPunctuation{"/", p_division, op_multiplicative},

	CPunctuation{"=", p_assign, op_assignment},

	CPunctuation{"|", p_bitwise_or, op_bitwise_or},
	CPunctuation{"^", p_bitwise_xor, op_bitwise_xor},
	CPunctuation{"&", p_bitwise_and, op_bitwise_and},

	CPunctuation{"(", p_par_open, op_postfix},
	CPunctuation{")", p_par_close, op_failure},

	CPunctuation{"[", p_bracket_open, op_postfix},
	CPunctuation{"]", p_bracket_close, op_failure},

	CPunctuation{"{", p_curlybracket_open, op_failure},
	CPunctuation{"}", p_curlybracket_close, op_failure},

	CPunctuation{",", p_comma},
	CPunctuation{".", p_period, op_postfix},
	CPunctuation{";", p_semicolon },
	CPunctuation{":", p_colon, op_conditional2},
	CPunctuation{"?", p_question_mark, op_conditional},
	CPunctuation{"!", p_exclamation, op_unary}

};