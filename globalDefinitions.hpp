#pragma once

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;

enum EValueType : unsigned char
{
	t_undefined,
	t_int,
	t_double
};