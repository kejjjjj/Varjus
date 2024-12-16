#pragma once
#include <string>
#include <memory>

#include "globalDefinitions.hpp"
#include "linter/modules/references.hpp"

enum EMemoryIdentifierType
{
	mi_variable,
	mi_function
};

struct CMemoryIdentifier : public CCrossModuleReference
{
	CMemoryIdentifier() = delete;
	CMemoryIdentifier(const std::string& name, const CCrossModuleReference& ref)
		: CCrossModuleReference(ref), m_sName(name) { }

	virtual ~CMemoryIdentifier() = default;

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept = 0;

	std::string m_sName;
};

