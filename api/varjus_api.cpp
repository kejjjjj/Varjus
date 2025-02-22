#include "varjus_api.hpp"

#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"
#include "linter/tokenizer.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "types/std/console.hpp"
#include "types/std/math.hpp"
#include "types/internal/object_declarations.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

struct CVarjusInternals
{ 


    static void Init() {
        m_bScriptLoaded = failure;
        m_sErrorMessage.clear();
#ifndef RUNNING_TESTS
        if (m_pReturnValue)
            m_pReturnValue->Release();
        m_pReturnValue = nullptr;
#endif
    }

    static std::string m_sErrorMessage;
    static Success m_bScriptLoaded;
    static IValue* m_pReturnValue;
};

std::string CVarjusInternals::m_sErrorMessage{};
Success CVarjusInternals::m_bScriptLoaded{};
IValue* CVarjusInternals::m_pReturnValue{};

static void InitVarjus()
{
    CVarjusInternals::Init();
    CLinterErrors::ClearErrorStack();
    CModule::ResetEverythingStatic();
    CFileContext::ResetGlobally();
    CProgramRuntime::Cleanup();
}

void Varjus::UseStdLibrary()
{
    CBuiltInObjects::Reset();
    Varjus::AddNewGlobalObject("console", CConsoleValue::ConstructMethods);
    Varjus::AddNewGlobalObject("math", CMathValue::ConstructMethods, CMathValue::ConstructProperties);
}

void Varjus::Cleanup() {
    InitVarjus();
    CBuiltInObjects::Reset();
}

Success Varjus::LoadScriptFromFile(const std::string& fullFilePath)
{
    InitVarjus();

    try {
        auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(fullFilePath);
        auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(begin, end, fullFilePath);

        if (!linter.Parse()) {
            CLinterErrors::PushError("couldn't parse the input file");
            return failure;
        }
        return CVarjusInternals::m_bScriptLoaded = success;
    }
    catch (CLinterError& ex) {
        CVarjusInternals::m_sErrorMessage = ex.what();
    } catch (std::exception& ex) {
        CVarjusInternals::m_sErrorMessage = "Unexpected exception: " + std::string(ex.what());
    }

    return failure;
}
IValue* Varjus::ExecuteScript()
{
    if (CVarjusInternals::m_bScriptLoaded == failure) {
        CLinterErrors::PushError("couldn't execute the script due to a missing script or syntax error");
        return nullptr;
    }

    try {

        CProgramRuntime runtime(CModule::ToRuntimeModules());
        return CVarjusInternals::m_pReturnValue = runtime.Execute();

    } catch (CRuntimeError& ex) {
        CVarjusInternals::m_sErrorMessage = ex.what();
    } catch (std::exception& ex) {
        CVarjusInternals::m_sErrorMessage = "Unexpected exception: " + std::string(ex.what());
    }

    return nullptr;

}
std::optional<std::string> Varjus::GetErrorMessage()
{
    return CVarjusInternals::m_sErrorMessage.size() 
        ? std::make_optional<std::string>(CVarjusInternals::m_sErrorMessage) 
        : std::nullopt;
}

void Varjus::PrintMemoryUsage() {
    CProgramRuntime::PrintAllLeaks();
}

void Varjus::AddNewGlobalObject(const std::string& name,
    const OptionalCtor<BuiltInMethod_t>& createMethods,
    const OptionalCtor<BuiltInProperty_t>& createProperties) 
{
    CBuiltInObjects::AddNewGlobalObject(name, createMethods, createProperties);
    
}
