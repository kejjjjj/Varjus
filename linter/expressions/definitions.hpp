#pragma once

#include <vector>
#include <string>
#include <memory>

enum Success : signed char;
class CPunctuationToken;
class CToken;

using LinterIterator = std::vector<CToken*>::iterator;

template<typename T>
using UniquePointerVector = std::vector<std::unique_ptr<T>>;

template<class ContainerType>
class CLinter
{
	static_assert(std::is_same_v<ContainerType, CToken> || std::is_same_v<ContainerType, CPunctuationToken>, 
		"ContainerType must have CToken as the base");
public:
	CLinter() = delete;

	explicit CLinter(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end){}
	virtual ~CLinter() = default;

protected:
	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;

	std::vector<const ContainerType*> m_oTokens;
};