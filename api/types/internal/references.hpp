#pragma once

#include <cstddef>
#include <unordered_map>

// a way to identify where this has been declared
struct CCrossModuleReference
{
	CCrossModuleReference() = default;
	explicit CCrossModuleReference(std::size_t index) : m_uIndex(index) {}
	explicit CCrossModuleReference(bool differentModule, std::size_t moduleIndex, std::size_t identifierIndex) :
		m_bBelongsToDifferentModule(differentModule), m_uModuleIndex(moduleIndex), m_uIndex(identifierIndex) {
	}

	bool m_bBelongsToDifferentModule{ false };
	std::size_t m_uModuleIndex{};
	std::size_t m_uIndex{};

	[[nodiscard]] inline bool operator==(const CCrossModuleReference& other) const {

		return m_bBelongsToDifferentModule == other.m_bBelongsToDifferentModule &&
			m_uModuleIndex == other.m_uModuleIndex &&
			m_uIndex == other.m_uIndex;
	}

};

struct CCrossModuleReferenceHasher
{
	[[nodiscard]] inline std::size_t operator()(const CCrossModuleReference& obj) const {

		std::hash<std::size_t> indexHash;
		std::hash<std::size_t> moduleIndexHash;
	
		return indexHash(obj.m_uIndex) ^ (moduleIndexHash(obj.m_uModuleIndex) << 1);
	}
};
