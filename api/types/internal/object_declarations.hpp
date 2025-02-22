#pragma once

#define FORWARD_DECLARE_FUNCTION(Name) \
[[nodiscard]] IValue* Name(CRuntimeContext* const ctx, IValue* _this, const IValues& values)
#define FORWARD_DECLARE_METHOD(Name) FORWARD_DECLARE_FUNCTION(Name)

#define DEFINE_FUNCTION(Name)\
IValue* Name([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& args)
#define DEFINE_METHOD(Name) DEFINE_FUNCTION(Name)


#define FORWARD_DECLARE_PROPERTY(Name)\
[[nodiscard]] IValue* Name(IValue* _this)

#define DEFINE_PROPERTY(Name)\
IValue* Name([[maybe_unused]] IValue* _this)

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class IValue;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;
using Method_t = IValue*(*)(struct CRuntimeContext* const, IValue*, const IValues&);
using Property_t = IValue*(*)(IValue*);

//msvc is unable to use the move constructor for the unique_ptr, so it has to be explicitly defined pagman
struct BuiltInMethod_t : std::unordered_map<std::size_t, std::unique_ptr<class CBuiltInRuntimeFunction>>
{
	using unordered_map::unordered_map;
	~BuiltInMethod_t();
	BuiltInMethod_t(const BuiltInMethod_t&) = delete;
	BuiltInMethod_t(BuiltInMethod_t&&) = default;
	BuiltInMethod_t& operator=(const BuiltInMethod_t&) = delete;
	BuiltInMethod_t& operator=(BuiltInMethod_t&&) = default;

	void AddMethod(const std::string& name, Method_t method, std::size_t numArgs);
};

struct BuiltInProperty_t : std::unordered_map<std::size_t, Property_t>
{
	void AddProperty(const std::string& name, Property_t property);
};

#define METHOD_BIND(v, value) \
v->MakeShared();\
v->Internal()->SetCallable(m_oMethods->at(memberIdx).get());\
v->MakeImmutable();\
v->Internal()->Bind(value);\

constexpr auto UNCHECKED_PARAMETER_COUNT = std::numeric_limits<std::size_t>::max();

