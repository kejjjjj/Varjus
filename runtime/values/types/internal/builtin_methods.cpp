#include "builtin_methods.hpp"
#include "runtime/values/types/array.hpp"
#include "runtime/values/types/string.hpp"

#include <iostream>

using Array = CBuiltInMethods<CArrayValue>;
using String = CBuiltInMethods<CStringValue>;

template<> Array::LookupType Array::m_oMethodLookup = {};
template<> String::LookupType String::m_oMethodLookup = {};

void BuiltInMethods::Setup(const CProgramContext* context) {

	static auto arrayStuff = CArrayValue::ConstructMethods();
	Array::Initialize(context, arrayStuff);

	static auto stringStuff = CStringValue::ConstructMethods();
	String::Initialize(context, stringStuff);

}
