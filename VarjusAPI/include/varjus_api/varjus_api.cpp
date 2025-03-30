#include "varjus_api.hpp"

#include "linter/context.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/modules/module.hpp"
#include "linter/tokenizer.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "types/std/stdconsole.hpp"
#include "types/std/stdmath.hpp"
#include "types/std/stdnumber.hpp"
#include "types/std/stdregex.hpp"
#include "types/std/stddate.hpp"
#include "types/std/stdfs.hpp"

#include "types/internal/object_declarations.hpp"
#include "types/internal/callbacks.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
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

Success Varjus::State::UseStdLibrary(const std::span<VarjusString>& ignore)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = VSL("Varjus::State::UseStdLibrary(): no linter context... did you forget to create a new state?");
        return failure;
    }

    std::unordered_map<VarjusString, void(*)(ObjectDeclaration_t&)> kv = {
        { VSL("console"), CConsoleValue::Construct   },
        { VSL("math"),    CMathValue::Construct      },
        { VSL("number"),  CStdNumberValue::Construct },
        { VSL("regex"),   CStdRegex::Construct       },
        { VSL("date"),    CStdDateValue::Construct   },
        { VSL("fs"),      CStdFsValue::Construct     }
    };

    for (const auto& [k, v] : kv) {
        if (std::ranges::find(ignore, k) != ignore.end())
            continue;

        m_pLinter->m_oBuiltInObjects->AddNewStaticObject(k, v);
    }

    return success;
}

Success Varjus::State::LoadScriptFromFile(const VarjusString& fullFilePath, EncodingType fileEncoding)
{

    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = VSL("Varjus::State::LoadScriptFromFile(): no linter context... did you forget to create a new state?");
        return failure;
    }

    try {
        auto uniqueTokens = CBufferTokenizer::ParseFileFromFilePath(m_pLinter.get(), fullFilePath, fileEncoding);
        auto tokens = CBufferTokenizer::ConvertTokensToReadOnly(uniqueTokens);
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(m_pLinter.get(), begin, end, fullFilePath);

        if (!linter.Parse()) {
            m_pLinter->PushError(VSL("couldn't parse the input file"));
            return failure;
        }
        return m_bScriptLoaded = success;
    }
    catch (CLinterError& ex) {
        m_sErrorMessage = ex.what();
    } 
    catch (std::exception& ex) {
        std::cerr << ex.what() << '\n';
        m_sErrorMessage = VSL("Unexpected exception occurred");
    } 
    catch (...) {
        m_sErrorMessage = VSL("Unexpected exception occurred");
    }

    return failure;
}
Success Varjus::State::LoadScript(const VarjusString& script)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = VSL("Varjus::State::LoadScript(): no linter context... did you forget to create a new state?");
        return failure;
    }

    try {
        CBufferTokenizer buffer(m_pLinter.get(), script);

        if (!buffer.Tokenize()) {
            m_sErrorMessage = VSL("Varjus::State::LoadScript(): tokenization failure.. syntax error?");
            return failure;
        }

        auto tokens = buffer.GetTokens();
        auto begin = tokens.begin();
        auto end = tokens.end();

        CBufferLinter linter(m_pLinter.get(), begin, end);

        if (!linter.Parse()) {
            m_pLinter->PushError(VSL("couldn't parse the input script"));
            return failure;
        }

        return m_bScriptLoaded = success;
    }
    catch (CLinterError& ex) {
        m_sErrorMessage = ex.what();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << '\n';
        m_sErrorMessage = VSL("Unexpected exception occurred");
    } 
    catch (...) {
        m_sErrorMessage = VSL("Unexpected exception occurred");
    }

    return failure;
}
IValue* Varjus::State::ExecuteScript(void(*callback)(CProgramRuntime* const ctx, IValues& receiver))
{

    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = VSL("Varjus::State::ExecuteScript(): no linter context... did you forget to create a new state?");
        return nullptr;
    }

    if (m_bScriptLoaded == failure) {
        m_pLinter->PushError(VSL("Varjus::State::ExecuteScript(): the script wasn't loaded successfully - check return values!!!"));
        return nullptr;
    }

    try {
        if (auto modules = m_pLinter->GetModules()) {
            m_pRuntime = std::make_unique<CProgramRuntime>(std::move(m_pLinter), modules->ToRuntimeModules());
        } else {
            m_sErrorMessage = VSL("Varjus::State::ExecuteScript(): no modules... internal bug?");
            return nullptr;
        }
        
        if (!m_pRuntime) {
            m_sErrorMessage = VSL("Varjus::State::ExecuteScript(): call to std::make_unique failed... internal bug?");
            return nullptr;
        }

        if (callback)
            callback(m_pRuntime.get(), m_oArgs);

        IValues args;
        if (m_oArgs.size()) {
            args.push_back(CArrayValue::Construct(m_pRuntime.get(), std::move(m_oArgs)));
        }

        return m_pReturnValue = m_pRuntime->Execute(args);

    } catch (CRuntimeError& ex) {
        m_sErrorMessage = ex.what();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << '\n';
        m_sErrorMessage = VSL("Unexpected exception occurred");
    } 
    catch (...) {
        m_sErrorMessage = VSL("Unexpected exception occurred");
    }

    return nullptr;

}
IValue* Varjus::State::GetReturnValue() const noexcept
{
    if (!m_pRuntime)
        return nullptr;

    return m_pReturnValue;
}

Success Varjus::State::Abort()
{
    if (!m_pRuntime) {
        m_sErrorMessage = VSL("Varjus::State::Abort(): No existing runtime");
        return failure;
    }

    m_pRuntime->Abort();
    return success;
}
Success Varjus::State::HasFinished()
{
    if (!m_pRuntime || !m_pReturnValue)
        return failure;

    return success;
}
std::optional<VarjusString> Varjus::State::GetErrorMessage() const noexcept {
    return m_sErrorMessage.size() ? std::make_optional<VarjusString>(m_sErrorMessage) : std::nullopt;
}

Success Varjus::State::AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructor)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInObjects) {
        m_sErrorMessage = VSL("Varjus::State::AddNewStaticObject(): no linter context... did you forget to create a new state?");
        return failure;
    }

    m_pLinter->m_oBuiltInObjects->AddNewStaticObject(name, constructor);
    return success;
}

Success Varjus::State::AddNewCallback(const VarjusString& name, const Function_t& callback, std::size_t numArgs)
{
    if (!m_pLinter || !m_pLinter->m_oBuiltInFunctions) {
        m_sErrorMessage = VSL("Varjus::State::AddNewCallback(): no linter context... did you forget to create a new state?");
        return failure;
    }

    m_pLinter->m_oBuiltInFunctions->AddNewGlobalCallable(name, callback, numArgs);
    return success;
}
