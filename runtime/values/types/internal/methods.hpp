#pragma once

#define FORWARD_DECLARE_METHOD(Name) \
[[nodiscard]] IValue* Name(CRuntimeContext* const ctx, IValue* _this, const IValues& values)

#define DEFINE_METHOD(Name)\
IValue* Name([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& newValues)

#define METHOD_TYPEDEF \
IValue*(*)(CRuntimeContext* const, IValue*, const IValues&)

#define METHOD_AS_VARIABLE(name) \
IValue*(*name)(CRuntimeContext* const, IValue*, const std::vector<IValue*>&)

#define DECLARE_BUILT_IN_METHODS \
std::unordered_map<std::size_t, std::unique_ptr<class CBuiltInRuntimeFunction>>

#define METHOD_BIND(v, value) \
v->MakeShared();\
v->Internal()->SetCallable(m_oMethods.at(memberIdx).get());\
v->MakeImmutable();\
v->Internal()->Bind(value);\

#define ADD_METHOD(name, method, numArgs)\
if(CFileContext::m_oAllMembers.Contains(name)){\
	const auto id = CFileContext::m_oAllMembers.At(name);\
	if(!m_oMethods.contains(id))\
		m_oMethods[id] = std::make_unique<CBuiltInRuntimeFunction>(method, numArgs);\
}\

constexpr auto UNCHECKED_PARAMETER_COUNT = std::numeric_limits<std::size_t>::max();
