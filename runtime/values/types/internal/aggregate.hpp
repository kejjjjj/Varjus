#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

class IValue;
class CVariable;
class AbstractSyntaxTree;

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

#define ARRAY_LENGTH std::size_t(0)



class CAggregate
{
public:
	CAggregate() = default;

	void Release();
	void Setup(const std::vector<ElementIndex>& elements);

	[[maybe_unused]] CVariable* AddAttribute(ElementIndex elem);
	void AddAttribute(ElementIndex elem, IValue* value);

	[[nodiscard]] IValue* ElementLookup(GlobalMemberIndex index) const;

	[[nodiscard]] auto& Iterator() { return m_oIndexLookup; }

protected:
	std::unordered_map<ElementIndex, CVariable*> m_oIndexLookup;
};
