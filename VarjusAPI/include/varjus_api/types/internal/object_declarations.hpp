#pragma once

namespace Varjus {
    struct CRuntimeContext;
    class CProgramRuntime;
}
#include <unordered_map>
#include <memory>

#include <vector>
#include <limits>

class IValue;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;
class CBuiltInRuntimeMethod;

#define VARJUS_DEFINE_STATIC_OBJECT(Name, receiver)\
void Name(Varjus::ObjectDeclaration_t& receiver)
#define VARJUS_DEFINE_PROPERTY(Name, ctx, _this)\
IValue* Name(Varjus::CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this)
#define VARJUS_DEFINE_METHOD(Name, ctx, _this, args)\
IValue* Name(Varjus::CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& args)
#define VARJUS_DEFINE_CALLBACK(name, ctx, args)\
IValue* name(Varjus::CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args)
#define VARJUS_DEFINE_ARGS(Name, ctx, args)\
void Name(Varjus::CProgramRuntime* const ctx, IValues& receiver)

namespace Varjus {
    class CProgramInformation;

    [[maybe_unused]] constexpr auto UNCHECKED_PARAMETER_COUNT = std::numeric_limits<std::size_t>::max();

    using Method_t = IValue * (*)(Varjus::CRuntimeContext* const, IValue*, const IValues&);
    using Property_t = IValue * (*)(Varjus::CRuntimeContext* const, IValue*);
}

#include "varjus_api/internal/structure.hpp"

namespace Varjus {

    //msvc is unable to use the move constructor for the unique_ptr, so it has to be explicitly defined pagman
    struct BuiltInMethod_t : std::unordered_map<std::size_t, std::unique_ptr<CBuiltInRuntimeMethod>>
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
        VARJUS_NONCOPYABLE(ObjectDeclaration_t);
        friend class CRuntimeModule;

        ObjectDeclaration_t(CProgramInformation* const ptr)
            : m_oMethods(ptr), m_oProperties(ptr) {
        }

        void AddMethod(const VarjusString& name, Method_t method, std::size_t numArgs) {
            m_oMethods.AddMethod(name, method, numArgs);
        }
        void AddProperty(const VarjusString& name, Property_t property) {
            m_oProperties.AddProperty(name, property);
        }

    private:
        BuiltInMethod_t m_oMethods;
        BuiltInProperty_t m_oProperties;
    };
}



