#include "varjus_api.hpp"

#include "linter/context.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"
#include "linter/tokenizer.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/structure.hpp"
#include "types/std/console.hpp"
#include "types/std/math.hpp"
#include "types/internal/object_declarations.hpp"
#include "types/internal/callbacks.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

Varjus::State::State() 
    : m_pLinter(std::make_unique<CProgramInformation>()) {}
Varjus::State::~State() {

#ifndef RUNNING_TESTS
    if (m_pReturnValue)
        m_pReturnValue->Release();
#endif

}

Success Varjus::State::UseStdLibrary()
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::UseStdLibrary(): no linter context... did you forget to create a new state?";
        return failure;
    }

    auto thing = std::make_optional(std::function(CConsoleValue::ConstructMethods));

    m_pLinter->m_oBuiltInObjects->AddNewGlobalObject("console", CConsoleValue::ConstructMethods);
    m_pLinter->m_oBuiltInObjects->AddNewGlobalObject("math", CMathValue::ConstructMethods, CMathValue::ConstructProperties);

    return success;
}

Success Varjus::State::LoadScriptFromFile(const std::string& fullFilePath)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::LoadScriptFromFile(): no linter context... did you forget to create a new state?";
        return failure;
    }

    try {
        auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(m_pLinter.get(), fullFilePath);
        auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(m_pLinter.get(), begin, end, fullFilePath);

        if (!linter.Parse()) {
            m_pLinter->PushError("couldn't parse the input file");
            return failure;
        }
        return m_bScriptLoaded = success;
    }
    catch (CLinterError& ex) {
        m_sErrorMessage = ex.what();
    } catch (std::exception& ex) {
        m_sErrorMessage = "Unexpected exception: " + std::string(ex.what());
    }

    return failure;
}
IValue* Varjus::State::ExecuteScript()
{

    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::ExecuteScript(): no linter context... did you forget to create a new state?";
        return nullptr;
    }

    if (m_bScriptLoaded == failure) {
        m_pLinter->PushError("Varjus::State::ExecuteScript(): the script wasn't loaded successfully - check return values!!!");
        return nullptr;
    }

    try {
        if (auto modules = m_pLinter->GetModules()) {
            m_pRuntime = std::make_unique<CProgramRuntime>(std::move(m_pLinter), modules->ToRuntimeModules());
        } else {
            m_sErrorMessage = "Varjus::State::ExecuteScript(): no modules... internal bug?";
            return nullptr;
        }
        
        if (!m_pRuntime) {
            m_sErrorMessage = "Varjus::State::ExecuteScript(): call to std::make_unique failed... internal bug?";
            return nullptr;
        }

        return m_pReturnValue = m_pRuntime->Execute();

    } catch (CRuntimeError& ex) {
        m_sErrorMessage = ex.what();
    } catch (std::exception& ex) {
        m_sErrorMessage = "Unexpected exception: " + std::string(ex.what());
    }

    return nullptr;

}
std::optional<std::string> Varjus::State::GetErrorMessage() {
    return m_sErrorMessage.size() ? std::make_optional<std::string>(m_sErrorMessage) : std::nullopt;
}

Success Varjus::State::AddNewGlobalObject(const std::string& name,
    const OptionalCtor<BuiltInMethod_t>& createMethods,
    const OptionalCtor<BuiltInProperty_t>& createProperties)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::AddNewGlobalObject(): no linter context... internal bug?";
        return failure;
    }

    m_pLinter->m_oBuiltInObjects->AddNewGlobalObject(name, createMethods, createProperties);
    return success;
}

Success Varjus::State::AddNewCallback(const std::string& name, const Function_t& callback, std::size_t numArgs)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInFunctions) {
        m_sErrorMessage = "Varjus::State::AddNewCallback(): no linter context... internal bug?";
        return failure;
    }

    m_pLinter->m_oBuiltInFunctions->AddNewGlobalCallable(name, callback, numArgs);
    return success;
}
