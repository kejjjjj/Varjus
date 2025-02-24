#pragma once

#include <map>
#include <vector>
#include <memory>
#include <string>

class IValue;
class CVariable;
class AbstractSyntaxTree;

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

class CProgramRuntime;

class CAggregate
{
public:
	CAggregate() = default;
	virtual ~CAggregate() = default;


	constexpr void SetModuleIndex(std::size_t i) noexcept { m_uModuleIndex = i; }

	void Release();
	void Setup(CProgramRuntime* const runtime, std::size_t moduleIndex, const std::vector<ElementIndex>& elements);

	[[maybe_unused]] virtual CVariable* AddAttribute(ElementIndex elem);
	void AddAttribute(ElementIndex elem, IValue* value);
	[[nodiscard]] bool RemoveAttribute(ElementIndex elem);

	[[nodiscard]] IValue* ElementLookup(GlobalMemberIndex index) const;
	[[nodiscard]] bool Contains(const std::string& item) const;
	[[nodiscard]] IValue* Get(const std::string& item) const;

	[[nodiscard]] auto Length() const noexcept { return m_oIndexLookup.size(); }

	constexpr void SetAllocator(CProgramRuntime* alloc) noexcept { m_pAllocator = alloc; }
	[[nodiscard]] constexpr auto GetAllocator() const noexcept { return m_pAllocator; }

#ifdef RUNNING_TESTS
	[[nodiscard]] IValue* ElementLookupNoExcept(GlobalMemberIndex index) const noexcept;
#endif
	[[nodiscard]] auto& Iterator() { return m_oIndexLookup; }

protected:
	std::map<ElementIndex, CVariable*> m_oIndexLookup;
	std::size_t m_uModuleIndex{};
	CProgramRuntime* m_pAllocator{ nullptr };
};
