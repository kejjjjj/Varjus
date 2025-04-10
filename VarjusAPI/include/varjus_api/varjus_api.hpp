#pragma once

/***********************************************************************
ALWAYS CHECK RETURN VALUES!!!!
***********************************************************************/

#define VARJUS_API
#define __VARJUS_ND [[nodiscard]]

#include "varjus_api/internal/structure.hpp"
#include "types/internal/callbacks.hpp"
#include "types/internal/objects.hpp"
#include "types/types.hpp"

#include <optional>
#include <span>


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
        // ignore: a container of standard object names which will be ignored (e.g. "fs")
        VARJUS_API __VARJUS_ND Success UseStdLibrary(const std::span<VarjusString>& ignore = {});

        //This function expects a full file path, not a relative one
        //When using e_auto (unicode builds), make sure that the file has the BOM
        VARJUS_API __VARJUS_ND Success LoadScriptFromFile(const VarjusString& fullFilePath, EncodingType characterSet=e_utf8);

        //This function expects a script
        //It should be noted that modules cannot be used as there is no working directory
        VARJUS_API __VARJUS_ND Success LoadScript(const VarjusString& script);

        //Synchronous
        //Call me after you have loaded a script with LoadScriptFromFile or LoadScript
        //Don't do any memory management to the return value as it's managed by the API
        // addArgs: a callback to a function where the user can add their own launch arguments
        VARJUS_API __VARJUS_ND IValue* ExecuteScript(void(*addArgs)(CProgramRuntime* const ctx, IValues& receiver) = 0);

        //Get the return value from the main function
        //Use this in an asynchronous context
        VARJUS_API __VARJUS_ND IValue* GetReturnValue() const noexcept;

        //Asynchronous
        //Stop execution, makes ExecuteScript try to exit as soon as possible
        //Use the "HasFinished" method to wait until the execution ends if necessary
        VARJUS_API __VARJUS_ND Success Abort();

        //The script is no longer executing
        VARJUS_API __VARJUS_ND Success HasFinished();

        //When a function doesn't return a success
        VARJUS_API __VARJUS_ND std::optional<VarjusString> GetErrorMessage() const noexcept;

        //Declare a new global variable with its custom methods and properties (callbacks)
        VARJUS_API __VARJUS_ND Success AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructor);

        //Declare a C++ function that can be called from the script
        VARJUS_API __VARJUS_ND Success AddNewCallback(const VarjusString& name, const Function_t& callback, std::size_t numArgs);

    private: // All of this is managed by the class, so there should never be a need to publicly access these :)

        VarjusString m_sErrorMessage;
        Success m_bScriptLoaded{ failure };
        std::unique_ptr<CProgramInformation> m_pLinter;
        std::unique_ptr<CProgramRuntime> m_pRuntime;
        IValue* m_pReturnValue{ nullptr };
        IValues m_oArgs;
    };

}
