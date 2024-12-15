#pragma once
#include <string>

#include "globalDefinitions.hpp"

enum EMemoryIdentifierType
{
	mi_variable,
	mi_function
};

struct CMemoryIdentifier
{
	CMemoryIdentifier() = default;
	CMemoryIdentifier(const std::string& name, std::size_t index)
		: m_sName(name), m_uIndex(index) {
	}
	virtual ~CMemoryIdentifier() = default;

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept = 0;

	std::string m_sName;
	std::size_t m_uIndex{};

	bool m_bBelongsToDifferentModule{ false };
	std::size_t m_uOtherModuleIndex{};
	std::size_t m_uOtherModuleIdentifierIndex{};

};

