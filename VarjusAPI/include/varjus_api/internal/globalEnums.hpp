#pragma once

namespace Varjus {
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
		e_unknown,       // Encoding is unknown  
		e_auto,          // Try to auto-detect encoding (BOM-based)  

		e_ansi,          // Windows: system code page | Linux/macOS: assume UTF-8  
		e_utf8,          // UTF-8 without BOM  
		e_utf16le,       // UTF-16 Little Endian  
		e_utf16be,       // UTF-16 Big Endian  

	};
}