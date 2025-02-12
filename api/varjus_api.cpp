#include "varjus_api.hpp"
#include "linter/modules/module.hpp"
#include "runtime/values/types/internal_objects/internal_objects2.hpp"

void Varjus::Init()
{
    CModule::ResetEverythingStatic();
    CFileContext::ResetGlobally();
    CBuiltInObjects::Reset();
}
