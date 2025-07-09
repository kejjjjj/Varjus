#pragma once

#include <map>
#include <vector>
#include <memory>


#include "varjus_api/internal/globalDefinitions.hpp"


class IValue;
class CVariable;
class CStringIntegerHashMap;

using __GlobalMemberIndex = std::size_t;
using __ElementIndex = std::size_t;

namespace Varjus {
	class CProgramRuntime;
}
class CAggregate
{
public:
	CAggregate() = default;
	virtual ~CAggregate() = default;


	void Release();
	void Setup(Varjus::CProgramRuntime* const runtime, const std::vector<__ElementIndex>& elements);

	[[maybe_unused]] virtual CVariable* AddAttribute(__ElementIndex elem);
	void AddAttribute(__ElementIndex elem, IValue* value);
	[[nodiscard]] bool RemoveAttribute(__ElementIndex elem);

	[[nodiscard]] IValue* ElementLookup(__GlobalMemberIndex index) const;
	[[nodiscard]] bool Contains(const VarjusString& item) const;
	[[nodiscard]] IValue* Get(const VarjusString& item) const;

	[[nodiscard]] auto Length() const noexcept { return m_oIndexLookup.size(); }

	constexpr void SetAllocator(Varjus::CProgramRuntime* alloc) noexcept { m_pAllocator = alloc; }
	constexpr void SetRuntimeInformation(CStringIntegerHashMap* members) noexcept { m_pAllMembers = members; }

	[[nodiscard]] constexpr auto GetAllocator() const noexcept { return m_pAllocator; }
	[[nodiscard]] constexpr auto& GetRuntimeInformation() const noexcept { return m_pAllMembers; }

#ifdef RUNNING_TESTS
	[[nodiscard]] IValue* ElementLookupNoExcept(__GlobalMemberIndex index) const noexcept;
#endif
	[[nodiscard]] auto& Iterator() { return m_oIndexLookup; }

protected:
	std::map<__ElementIndex, CVariable*> m_oIndexLookup;
	Varjus::CProgramRuntime* m_pAllocator{ nullptr };
	CStringIntegerHashMap* m_pAllMembers{ nullptr }; // a pointer to a list of all declared members... to save computing power
};
