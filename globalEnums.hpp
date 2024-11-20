#pragma once


enum Success : signed char
{
	failure,
	success
};

enum EValueType : unsigned char
{
	t_undefined,
	t_boolean,
	t_int,
	t_double,
	t_string,
	t_callable,
	t_array,
	t_object,
	t_member_callable,
};