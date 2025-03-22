#pragma once

/***********************************************************************
ALWAYS CHECK RETURN VALUES!!!!
***********************************************************************/

#define VARJUS_API
#define __ND [[nodiscard]]

#include "varjus_api/internal/structure.hpp"
#include "types/internal/callbacks.hpp"
#include "types/internal/objects.hpp"
#include "types/types.hpp"

#include <optional>


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
        //When using e_auto (unicode builds), make sure that the file has the BOM
        VARJUS_API __ND Success LoadScriptFromFile(const VarjusString& fullFilePath, EncodingType characterSet);

        //This function expects a script
        //It should be noted that modules cannot be used as there is no working directory
        VARJUS_API __ND Success LoadScript(const VarjusString& script);

        //Call me after you have loaded a script with LoadScriptFromFile or LoadScript
        //Don't do any memory management to the return value as it's managed by the API
        VARJUS_API __ND IValue* ExecuteScript();

        //When a function doesn't return a success
        VARJUS_API __ND std::optional<VarjusString> GetErrorMessage() const noexcept;

        //Declare a new global variable with its custom methods and properties (callbacks)
        VARJUS_API __ND Success AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructor);

        //Declare a C++ function that can be called from the script
        VARJUS_API __ND Success AddNewCallback(const VarjusString& name, const Function_t& callback, std::size_t numArgs);

    private: // All of this is managed by the class, so there should never be a need to publicly access these :)

        VarjusString m_sErrorMessage;
        Success m_bScriptLoaded{ failure };
        std::unique_ptr<CProgramInformation> m_pLinter;
        std::unique_ptr<CProgramRuntime> m_pRuntime;
        IValue* m_pReturnValue{ nullptr };
    };

}
