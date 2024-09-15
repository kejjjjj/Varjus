#pragma once

#include <vector>
#include <string>
#include <memory>

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;

enum Success : signed char;
class CPunctuationToken;
class CToken;
class CLinterOperatorParser;


template<typename T>
using VectorOf = std::vector<T>;

template<typename T>
using UniquePointerVector = VectorOf<std::unique_ptr<T>>;

using LinterIterator = VectorOf<CToken*>::iterator;

using OperatorIterator = VectorOf<CLinterOperatorParser*>::iterator;
using COperatorIterator = VectorOf<CLinterOperatorParser*>::const_iterator;

template<class ContainerType>
class CLinter
{
	static_assert(std::is_same_v<ContainerType, CToken> || std::is_same_v<ContainerType, CPunctuationToken>, 
		"ContainerType must have CToken as the base");
public:
	CLinter() = delete;

	explicit CLinter(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end){}
	virtual ~CLinter() = default;

	[[nodiscard]] bool IsEndOfBuffer() const noexcept { return m_iterPos == m_iterEnd; }

protected:
	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};

template<class Type>
class CLinterSingle : public CLinter<CToken>
{
public:
	CLinterSingle() = delete;
	explicit CLinterSingle(LinterIterator& pos, LinterIterator& end) : CLinter(pos, end) {}

	CLinterSingle operator=(const CLinterSingle&) = delete;

protected:
	const Type* m_pToken{};
};

template<class ContainerType>
class CVectorLinter : public CLinter<CToken>
{
public:
	CVectorLinter() = delete;
	explicit CVectorLinter(LinterIterator& pos, LinterIterator& end) : CLinter(pos, end) {}

	CVectorLinter operator=(const CVectorLinter&) = delete;
protected:
	VectorOf<const ContainerType*> m_oTokens;
};