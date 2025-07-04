#pragma once

struct CRuntimeContext;
#define FORWARD_DECLARE_FUNCTION(Name) \
[[nodiscard]] IValue* Name(CRuntimeContext* const ctx, const IValues& values)
#define DEFINE_FUNCTION(Name, args)\
IValue* Name([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args)


#define FORWARD_DECLARE_METHOD(Name)\
[[nodiscard]] IValue* Name(CRuntimeContext* const ctx, IValue* _this, const IValues& values)
#define DEFINE_METHOD(Name, args)\
IValue* Name([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& args)


#define FORWARD_DECLARE_PROPERTY(Name)\
[[nodiscard]] IValue* Name(CRuntimeContext* const ctx, IValue* _this)
#define DEFINE_PROPERTY(Name)\
IValue* Name([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this)


#include <unordered_map>
#include <memory>

#include <vector>
#include <limits>

[[maybe_unused]] constexpr auto UNCHECKED_PARAMETER_COUNT = std::numeric_limits<std::size_t>::max();

class IValue;
class CProgramInformation;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

using Method_t = IValue*(*)(struct CRuntimeContext* const, IValue*, const IValues&);
using Property_t = IValue*(*)(struct CRuntimeContext* const, IValue*);


#define VARJUS_DEFINE_STATIC_OBJECT(Name, receiver)\
void Name(ObjectDeclaration_t& receiver)
#define VARJUS_DEFINE_PROPERTY(Name, ctx, _this)\
IValue* Name(struct CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this)
#define VARJUS_DEFINE_METHOD(Name, ctx, _this, args)\
IValue* Name(struct CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& args)

#define VARJUS_DEFINE_ARGS(Name, ctx, args)\
void Name(class CProgramRuntime* const ctx, IValues& receiver)


#include "varjus_api/internal/structure.hpp"

//msvc is unable to use the move constructor for the unique_ptr, so it has to be explicitly defined pagman
struct BuiltInMethod_t : std::unordered_map<std::size_t, std::unique_ptr<class CBuiltInRuntimeMethod>>
{
    using unordered_map::unordered_map;

    BuiltInMethod_t() = delete;
    BuiltInMethod_t(CProgramInformation* const ptr) : m_pInfo(ptr) {}
    ~BuiltInMethod_t() = default;
    BuiltInMethod_t(const BuiltInMethod_t&) = delete;
    BuiltInMethod_t(BuiltInMethod_t&&) = default;
    BuiltInMethod_t& operator=(const BuiltInMethod_t&) = delete;
    BuiltInMethod_t& operator=(BuiltInMethod_t&&) = default;

    constexpr void SetContext(CProgramInformation* const info) { m_pInfo = info; }
    void AddMethod(const VarjusString& name, Method_t method, std::size_t numArgs);


    CProgramInformation* m_pInfo;
};

struct BuiltInProperty_t : std::unordered_map<std::size_t, Property_t>
{

    BuiltInProperty_t() = delete;
    BuiltInProperty_t(CProgramInformation* const ptr) : m_pInfo(ptr) {}

	constexpr void SetContext(CProgramInformation* const info) { m_pInfo = info; }
	void AddProperty(const VarjusString& name, Property_t property);

	CProgramInformation* m_pInfo{};
};

struct ObjectDeclaration_t
{
    NONCOPYABLE(ObjectDeclaration_t);
    friend class CRuntimeModule;

    ObjectDeclaration_t(CProgramInformation* const ptr) 
        : m_oMethods(ptr), m_oProperties(ptr) {}
    
    void AddMethod(const VarjusString& name, Method_t method, std::size_t numArgs) {
        m_oMethods.AddMethod(name, method, numArgs);
    }
    void AddProperty(const VarjusString& name, Property_t property){
        m_oProperties.AddProperty(name, property);
    }

private:
    BuiltInMethod_t m_oMethods;
    BuiltInProperty_t m_oProperties;
};

#define METHOD_BIND(v, methods, value) \
v->MakeShared();\
v->Internal()->SetCallable(methods->at(memberIdx).get());\
v->MakeImmutable();\
v->Internal()->Bind(value);\


