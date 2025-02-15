#include "varjus_api.hpp"
#include "linter/modules/module.hpp"
#include "types/internal/internal_objects.hpp"

#include "types/std/console.hpp"
#include "types/std/math.hpp"

void Varjus::UseStdLibrary()
{
    CBuiltInObjects::AddNewGlobalObject("console", CConsoleValue::ConstructMethods);
    CBuiltInObjects::AddNewGlobalObject("math", CMathValue::ConstructMethods, CMathValue::ConstructProperties);
}

void Varjus::Init()
{
    CModule::ResetEverythingStatic();
    CFileContext::ResetGlobally();
    CBuiltInObjects::Reset();
}
