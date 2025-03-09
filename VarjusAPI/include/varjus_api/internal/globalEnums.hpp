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
	t_uint,
	t_int,
	t_double,
	t_string,
	t_callable,
	t_array,
	t_object,
};

enum EncodingType : signed char 
{ 
	// !!! don't use me !!!
	e_unknown, 

	//automatically determine the encoding (requires that the input stream has a byte order mark)
	// !!! don't use me !!!
	e_auto, 

	e_utf8, 
	e_utf16le, 
	e_utf16be 

};
