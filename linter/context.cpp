#include "context.hpp"
#include "linter/modules/module.hpp"
#include "varjus_api/types/internal/objects.hpp"
#include "varjus_api/types/internal/callbacks.hpp"

using namespace Varjus;

CProgramInformation::CProgramInformation()
	: m_pModules(std::make_unique<CProjectModules>()),
	m_oBuiltInObjects(std::make_unique<Varjus::CBuiltInObjects>()),
	m_oBuiltInFunctions(std::make_unique<Varjus::CBuiltInFunctions>()) {}
CProgramInformation::~CProgramInformation() = default;

