#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

class IValue;
class CVariable;

using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

#define ARRAY_LENGTH std::size_t(0)



class CAggregate
{
public:
	CAggregate() = default;

	void Release();
	void Setup(const std::vector<ElementIndex>& elements);

	[[nodiscard]] IValue* ElementLookup(GlobalMemberIndex index) const;

protected:
	std::unordered_map<ElementIndex, CVariable*> m_oIndexLookup;
};
