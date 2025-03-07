#include <iostream>
#include "varjus_api/varjus_api.hpp"

#define __MU [[maybe_unused]]

void DefineObject(ObjectDeclaration_t& obj)
{
    
    obj.AddProperty("property", [](CRuntimeContext* const ctx, __MU IValue* _this) -> IValue* {
        return CStringValue::Construct(ctx->m_pRuntime, "hello!");
    });

    obj.AddMethod("method", [](CRuntimeContext* const ctx, __MU IValue* _this, __MU const IValues& args) -> IValue* {
        return CStringValue::Construct(ctx->m_pRuntime, std::format("method({})", args[0]->ValueAsString()));
    }, 1);

}

int main(int argc, char** argv)
{

    if (argc != 2) {
        std::cout << "usage: <file path>\n";
        return 1;
    }

    Varjus::State state;

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!state.UseStdLibrary() || !state.AddNewStaticObject("hello", DefineObject)) {
        std::cerr << "state error: " << GetError(state.GetErrorMessage()) << '\n';
        return 1;
    }

    if (!state.LoadScriptFromFile(argv[1])) {
        std::cerr << "syntax error: " << GetError(state.GetErrorMessage()) << '\n';
        return 1;
    }

    if (const auto returnValue = state.ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << std::endl;
    }
    else {
        std::cerr << "runtime error: " << GetError(state.GetErrorMessage()) << '\n';
        return 1;
    }

    return 0;
}