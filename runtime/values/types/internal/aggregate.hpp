#pragma once

#include <unordered_map>
#include <vector>

class IValue;

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

#define ARRAY_LENGTH std::size_t(0)

class CAggregate
{
public:
	CAggregate() = default;

	void Setup(const std::vector<ElementIndex>& elements);

	[[nodiscard]] IValue* ElementLookup(GlobalMemberIndex index) const;

protected:
	std::unordered_map<ElementIndex, IValue*> m_oIndexLookup;

};
