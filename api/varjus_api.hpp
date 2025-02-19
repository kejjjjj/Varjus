#pragma once

#define VARJUS_API
#define __ND [[nodiscard]]

#include "internal/globalDefinitions.hpp"
#include "types/types.hpp"

#include <string>
#include <optional>

/***********************************************************************
ALWAYS CHECK RETURN VALUES!!!!
***********************************************************************/

/***********************************************************************
Example copy paste code:

#include <iostream>
#include "api/varjus_api.hpp"

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
namespace Varjus
{
	//Call me first if you need access to standard objects such as math and console
	VARJUS_API void UseStdLibrary();

	//Call me when you don't need the api anymore
	VARJUS_API void Cleanup();

	//This function expects a full file path, not a relative one
	VARJUS_API __ND Success LoadScriptFromFile(const std::string& fullFilePath);
	
	//Call me after you have loaded a script with LoadScriptFromFile or LoadScript
	//Don't do any memory management to the return value as it's managed by the API
    //Just remember to call Varjus::Cleanup() after you don't need this value
	VARJUS_API __ND IValue* ExecuteScript();

	//When a function doesn't return a success
	VARJUS_API __ND std::optional<std::string> GetErrorMessage();

#if _DEBUG
	// Debug memory leaks
	// call me after Varjus::Cleanup()
	VARJUS_API void PrintMemoryUsage();
#endif
}

