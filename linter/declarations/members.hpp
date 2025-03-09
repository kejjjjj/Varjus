#pragma once

#include <unordered_map>


#include "varjus_api/internal/globalDefinitions.hpp"

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

/***********************************************************************
 > provides quick lookup values for object member names
***********************************************************************/
class CStringIntegerHashMap
{
public:
	[[maybe_unused]] std::size_t operator[](const VarjusString& key);

	[[nodiscard]] bool Contains(const VarjusString& key) const;
	[[nodiscard]] const VarjusString& At(const GlobalMemberIndex key) const;
	[[nodiscard]] GlobalMemberIndex At(const VarjusString& key) const;
	[[maybe_unused]] std::size_t Insert(const VarjusString& key);
	[[nodiscard]] auto& StringToIntIterator() const noexcept { return m_oStringToId; }

	void Clear() noexcept { m_oStringToId.clear(); m_oIdToString.clear(); }
	void Erase(const VarjusString& key);

private:
	[[nodiscard]] GlobalMemberIndex GetCurrentIndex() const;

	std::unordered_map<VarjusString, GlobalMemberIndex> m_oStringToId;
	std::unordered_map<GlobalMemberIndex, VarjusString> m_oIdToString;
};
