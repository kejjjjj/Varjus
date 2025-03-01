#include <iostream>

#include "fs/fs_io.hpp"
#include "fs/fs_globals.hpp"

#include "varjus_api/varjus_api.hpp"

#include "varjus_api/internal/runtime.hpp"

#define __MU [[maybe_unused]]

int ExitApp(int v)
{
    std::cout << "Press ENTER to exit\n";
    std::cin.get();
    return v;
}

void CreateHello(ObjectDeclaration_t& receiver)
{
    // add a method
    receiver.AddMethod("method", [](CRuntimeContext* const context, __MU IValue* _this, __MU const IValues& args) -> IValue* {
        return CStringValue::Construct(context->m_pRuntime, "this is a method");
    }, 0);

    // add a property
    receiver.AddProperty("property", [](CProgramRuntime* const runtime, __MU IValue* _this) -> IValue* {
        return CStringValue::Construct(runtime, "this is a static property");
    });

}

VARJUS_DEFINE_CALLBACK(CppFunc, ctx, args) {
    return CStringValue::Construct(ctx->m_pRuntime, "Hello from C++!");
}


int main()
{
    
    Varjus::State state;

    if (!state.AddNewStaticObject("hello", CreateHello))
        return 0;

    if (!state.AddNewCallback("cppFunc", CppFunc, 0))
        return 0;


    const auto reader = VarjusIOReader(DIRECTORY_SEPARATOR_CHAR + "scripts"s + DIRECTORY_SEPARATOR_CHAR + "script.var"s);

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!state.UseStdLibrary()) {
        std::cout << "state error: " << GetError(state.GetErrorMessage()) << '\n';
        return ExitApp(0);
    }

    if ([[maybe_unused]]auto buff = reader.IO_Read()) {
        if (!state.LoadScript("fn main() { return `hi: ${200 + 5}`; }")) {
            std::cout << "syntax error: " << GetError(state.GetErrorMessage()) << '\n';
            return ExitApp(0);
        }
    } else {
        std::cout << "couldn't read the file\n";
        return ExitApp(0);
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << '\n';
    } else {
        std::cout << "runtime error: " << GetError(state.GetErrorMessage()) << '\n';
    }

    return 1;
}

