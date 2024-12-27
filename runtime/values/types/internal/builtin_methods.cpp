#include "builtin_methods.hpp"
#include "runtime/values/types/array.hpp"
#include "runtime/values/types/string.hpp"
#include "runtime/values/types/internal_objects/console.hpp"
#include "runtime/values/types/internal_objects/math.hpp"

#include <iostream>

using Array = CBuiltInMethods<CArrayValue>;
using String = CBuiltInMethods<CStringValue>;
using Console = CBuiltInMethods<CConsoleValue>;
using Math = CBuiltInMethods<CMathValue>;

template<> Array::LookupType Array::m_oMethodLookup = {};
template<> String::LookupType String::m_oMethodLookup = {};
template<> Console::LookupType Console::m_oMethodLookup = {};
template<> Math::LookupType Math::m_oMethodLookup = {};

void BuiltInMethods::Setup(const CFileContext* context) {

	static auto arrayStuff = CArrayValue::ConstructMethods();
	Array::Initialize(context, arrayStuff);

	static auto stringStuff = CStringValue::ConstructMethods();
	String::Initialize(context, stringStuff);

	static auto consoleStuff = CConsoleValue::ConstructMethods();
	Console::Initialize(context, consoleStuff);

	static auto mathStuff = CMathValue::ConstructMethods();
	Math::Initialize(context, mathStuff);
}
