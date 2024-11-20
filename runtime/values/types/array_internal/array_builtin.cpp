#include "array_builtin.hpp"
#include "linter/context.hpp"
#include <stdexcept>
#include <functional>
std::unordered_map<ElementIndex, const CArrayBuiltInMethod*> CStaticArrayBuiltInMethods::m_oMethodLookup;
std::unordered_map<std::string, CArrayBuiltInMethod> CStaticArrayBuiltInMethods::m_oStaticMethods = {

	{"push", {1u, &CArrayValue::Push}}

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

IValue* CStaticArrayBuiltInMethods::CallMethod(CArrayValue* _this, const IValues& args, const CArrayBuiltInMethod* method){
	return std::mem_fn(method->memFn)(_this, args);
}
