#include "array_builtin.hpp"
#include "linter/context.hpp"
#include "runtime/exceptions/exception.hpp"

#include <stdexcept>
#include <functional>
#include <format>
std::unordered_map<ElementIndex, const CArrayBuiltInMethod*> CStaticArrayBuiltInMethods::m_oMethodLookup;
std::unordered_map<std::string, CArrayBuiltInMethod> CStaticArrayBuiltInMethods::m_oStaticMethods = {

	{"push",       {1u, &CArrayValue::Push}},
	{"push_front", {1u, &CArrayValue::PushFront}},
	{"pop",        {0u, &CArrayValue::Pop}},
	{"pop_front",  {0u, &CArrayValue::PopFront}},
	{"map",        {1u, &CArrayValue::Map}},
	{"find",       {1u, &CArrayValue::Find}},
	{"filter",     {1u, &CArrayValue::Filter}},
	{"contains",   {1u, &CArrayValue::Contains}},

};


void CStaticArrayBuiltInMethods::Initialize(const CProgramContext* context){
	for (auto& [name, id] : context->m_oAllMembers.StringToIntIterator()) {
		if (m_oStaticMethods.contains(name)) {
			const auto& [_, method] = *m_oStaticMethods.find(name);
			m_oMethodLookup[id] = &method;

		}
	}
}
const CArrayBuiltInMethod* CStaticArrayBuiltInMethods::LookupMethod(ElementIndex index) {
	try {
		return m_oMethodLookup.at(index);
	} catch ([[maybe_unused]]std::out_of_range& ex) {
		return nullptr;
	}
}

IValue* CStaticArrayBuiltInMethods::CallMethod(CFunction* const thisFunction,
	CArrayValue* _this, const IValues& args, const CArrayBuiltInMethod* method){
	
	if (method->m_uNumArguments != args.size())
		throw CRuntimeError(std::format("the method expected {} arguments instead of {}", method->m_uNumArguments, args.size()));

	
	return std::mem_fn(method->memFn)(_this, thisFunction, args);
}
