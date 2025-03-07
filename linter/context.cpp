#include "context.hpp"
#include "linter/modules/module.hpp"
#include "varjus_api/types/internal/objects.hpp"
#include "varjus_api/types/internal/callbacks.hpp"
#include "modules/module.hpp"

CProgramInformation::CProgramInformation()
	: m_pModules(std::make_unique<CProjectModules>()),
	m_oBuiltInObjects(std::make_unique<CBuiltInObjects>()),
	m_oBuiltInFunctions(std::make_unique<CBuiltInFunctions>()) {}
CProgramInformation::~CProgramInformation() = default;

