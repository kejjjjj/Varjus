#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

class IValue;
class CVariable;
class AbstractSyntaxTree;

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

#define LENGTH_PROPERTY std::size_t(0)

enum AggregateType
{
	at_object,
	at_array
};


class CAggregate
{
public:
	CAggregate() = default;
	virtual ~CAggregate() = default;

	[[nodiscard]] virtual constexpr AggregateType Type() const noexcept { return at_object; }

	constexpr void SetModuleIndex(std::size_t i) noexcept { m_uModuleIndex = i; }

	void Release();
	void Setup(std::size_t moduleIndex, const std::vector<ElementIndex>& elements);

	[[maybe_unused]] virtual CVariable* AddAttribute(ElementIndex elem);
	void AddAttribute(ElementIndex elem, IValue* value);

	[[nodiscard]] IValue* ElementLookup(GlobalMemberIndex index) const;

#ifdef RUNNING_TESTS
	[[nodiscard]] IValue* ElementLookupNoExcept(GlobalMemberIndex index) const noexcept;
#endif
	[[nodiscard]] auto& Iterator() { return m_oIndexLookup; }

protected:
	std::unordered_map<ElementIndex, CVariable*> m_oIndexLookup;
	std::size_t m_uModuleIndex{};
};


class CArrayAggregate : public CAggregate
{
public:
	[[nodiscard]] constexpr AggregateType Type() const noexcept override { return at_array; }

	[[maybe_unused]] CVariable* AddAttribute(ElementIndex elem) override;


private:
};