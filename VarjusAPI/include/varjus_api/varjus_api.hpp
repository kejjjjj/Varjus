#pragma once

#define VARJUS_API
#define __ND [[nodiscard]]

#include "varjus_api/internal/structure.hpp"
#include "types/internal/callbacks.hpp"
#include "types/internal/objects.hpp"
#include "types/types.hpp"


#include <optional>
#include <string>

/***********************************************************************
ALWAYS CHECK RETURN VALUES!!!!
***********************************************************************/

/***********************************************************************
Example copy paste code:

#include <iostream>
#include "varjus_api/varjus_api.hpp"

int ExitApp(int v)
{
    Varjus::Cleanup();
    Varjus::PrintMemoryUsage();
    std::cout << "Press any key to exit\n";
    std::cin.get();
    return v;
}

int main()
{
    Varjus::UseStdLibrary();

    const auto GetError = [](const std::optional<std::string>& errorMsg) {
        return errorMsg ? *errorMsg : "unknown error!";
    };

    if (!Varjus::LoadScriptFromFile("C:\\temp\\script.var")) {
        std::cout << "syntax error: " << GetError(Varjus::GetErrorMessage()) << '\n';
        return ExitApp(0);
    }

    if (const auto returnValue = Varjus::ExecuteScript()) {
        std::cout << "the program returned: " << returnValue->ToPrintableString() << '\n';
    } else {
        std::cout << "runtime error: " << GetError(Varjus::GetErrorMessage()) << '\n';
    }

    return ExitApp(1);

}
***********************************************************************/

class CProgramInformation;
class CProgramRuntime;

namespace Varjus
{
    struct State
    {
        NONCOPYABLE(State);

        State();
        ~State();

        //Call me before loading a script if you need access to standard objects such as math and console
        VARJUS_API __ND Success UseStdLibrary();

        //This function expects a full file path, not a relative one
        VARJUS_API __ND Success LoadScriptFromFile(const std::string& fullFilePath);

        //This function expects a script
        //It should be noted that modules cannot be used as there is no working directory
        VARJUS_API __ND Success LoadScript(const std::string& script);

        //Call me after you have loaded a script with LoadScriptFromFile or LoadScript
        //Don't do any memory management to the return value as it's managed by the API
        VARJUS_API __ND IValue* ExecuteScript();

        //When a function doesn't return a success
        VARJUS_API __ND std::optional<std::string> GetErrorMessage();

        //Declare a new global variable with its custom methods and properties (callbacks)
        VARJUS_API __ND Success AddNewStaticObject(const std::string& name,
            const OptionalCtor<void>& constructor = std::nullopt);

        //When this function is referenced in code, it calls the callback
        VARJUS_API __ND Success AddNewCallback(const std::string& name, const Function_t& callback, std::size_t numArgs);

    private: // All of this is managed by the class, so there should never be a need to publicly access these :)

        std::string m_sErrorMessage;
        Success m_bScriptLoaded{ failure };
        std::unique_ptr<CProgramInformation> m_pLinter;
        std::unique_ptr<CProgramRuntime> m_pRuntime;
        IValue* m_pReturnValue{ nullptr };
    };

}
