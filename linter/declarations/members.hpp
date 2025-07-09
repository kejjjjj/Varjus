#pragma once

#include <unordered_map>


#include "varjus_api/internal/globalDefinitions.hpp"

using __GlobalMemberIndex = std::size_t;
using __ElementIndex = std::size_t;

/***********************************************************************
 > provides quick lookup values for object member names
***********************************************************************/
class CStringIntegerHashMap
{
public:
	[[maybe_unused]] std::size_t operator[](const VarjusString& key);

	[[nodiscard]] bool Contains(const VarjusString& key) const;
	[[nodiscard]] const VarjusString& At(const __GlobalMemberIndex key) const;
	[[nodiscard]] __GlobalMemberIndex At(const VarjusString& key) const;
	[[maybe_unused]] std::size_t Insert(const VarjusString& key);
	[[nodiscard]] auto& StringToIntIterator() const noexcept { return m_oStringToId; }

	void Clear() noexcept { m_oStringToId.clear(); m_oIdToString.clear(); }
	void Erase(const VarjusString& key);

private:
	[[nodiscard]] __GlobalMemberIndex GetCurrentIndex() const;

	std::unordered_map<VarjusString, __GlobalMemberIndex> m_oStringToId;
	std::unordered_map<__GlobalMemberIndex, VarjusString> m_oIdToString;
};
