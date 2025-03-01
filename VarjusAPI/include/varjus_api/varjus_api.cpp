#include "varjus_api.hpp"

#include "linter/context.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"
#include "linter/tokenizer.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
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

    #if _MSC_VER && _DEBUG
        if (m_pRuntime && m_pReturnValue) {
            m_pRuntime->PrintAllLeaks();
        }
    #endif
#endif



}

Success Varjus::State::UseStdLibrary()
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::UseStdLibrary(): no linter context... did you forget to create a new state?";
        return failure;
    }

    m_pLinter->m_oBuiltInObjects->AddNewStaticObject("console", CConsoleValue::Construct);
    m_pLinter->m_oBuiltInObjects->AddNewStaticObject("math", CMathValue::Construct);

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
Success Varjus::State::LoadScript(const std::string& script)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::LoadScript(): no linter context... did you forget to create a new state?";
        return failure;
    }

    try {
        CBufferTokenizer buffer(m_pLinter.get(), script);

        if (!buffer.Tokenize()) {
            m_sErrorMessage = "Varjus::State::LoadScript(): tokenization failure.. syntax error?";
            return failure;
        }

        auto tokens = buffer.GetTokens();
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(m_pLinter.get(), begin, end);

        if (!linter.Parse()) {
            m_pLinter->PushError("couldn't parse the input script");
            return failure;
        }

        return m_bScriptLoaded = success;
    }
    catch (CLinterError& ex) {
        m_sErrorMessage = ex.what();
    }
    catch (std::exception& ex) {
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

Success Varjus::State::AddNewStaticObject(const std::string& name, const OptionalCtor<void>& constructor)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = "Varjus::State::AddNewStaticObject(): no linter context... did you forget to create a new state?";
        return failure;
    }

    m_pLinter->m_oBuiltInObjects->AddNewStaticObject(name, constructor);
    return success;
}

Success Varjus::State::AddNewCallback(const std::string& name, const Function_t& callback, std::size_t numArgs)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInFunctions) {
        m_sErrorMessage = "Varjus::State::AddNewCallback(): no linter context... did you forget to create a new state?";
        return failure;
    }

    m_pLinter->m_oBuiltInFunctions->AddNewGlobalCallable(name, callback, numArgs);
    return success;
}
