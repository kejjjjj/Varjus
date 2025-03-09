#pragma once

#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"
#include "varjus_api/types/internal/references.hpp"

enum EMemoryIdentifierType
{
	mi_variable,
	mi_function
};

struct CMemoryIdentifier : public CCrossModuleReference
{
	CMemoryIdentifier() = delete;
	CMemoryIdentifier(const VarjusString& name, const CCrossModuleReference& ref)
		: CCrossModuleReference(ref), m_sName(name) { }

	virtual ~CMemoryIdentifier() = default;

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept = 0;

	VarjusString m_sName;
};

