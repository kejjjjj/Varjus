#pragma once

/***********************************************************************
 > stuff that doesn't need to be exposed by the api, but is used internally
***********************************************************************/

#include "varjus_api/internal/globalDefinitions.hpp"

#include <vector>

namespace Varjus {
    struct CRuntimeContext;
    class CProgramRuntime;
}

class IValue;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

#define FORWARD_DECLARE_FUNCTION(Name) \
[[nodiscard]] IValue* Name(Varjus::CRuntimeContext* const ctx, const IValues& values)
#define DEFINE_FUNCTION(Name, args)\
IValue* Name([[maybe_unused]] Varjus::CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args)

#define FORWARD_DECLARE_METHOD(Name)\
[[nodiscard]] IValue* Name(Varjus::CRuntimeContext* const ctx, IValue* _this, const IValues& values)
#define DEFINE_METHOD(Name, args)\
IValue* Name([[maybe_unused]] Varjus::CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this, [[maybe_unused]] const IValues& args)

#define FORWARD_DECLARE_PROPERTY(Name)\
[[nodiscard]] IValue* Name(Varjus::CRuntimeContext* const ctx, IValue* _this)
#define DEFINE_PROPERTY(Name)\
IValue* Name([[maybe_unused]] Varjus::CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this)

#define VARJUS_METHOD_BIND(v, methods, value) \
v->MakeShared();\
v->Internal()->SetCallable(methods->at(memberIdx).get());\
v->MakeImmutable();\
v->Internal()->Bind(value);\

