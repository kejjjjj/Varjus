#pragma once

#include <vector>

#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"

class CPunctuationToken;
class CToken;
class CLinterOperator;

class CScope;
using WeakScope = std::weak_ptr<CScope>;

template<typename T>
using VectorOf = std::vector<T>;

template<typename T>
using UniquePointerVector = VectorOf<std::unique_ptr<T>>;

using LinterIterator = VectorOf<CToken*>::iterator;

using OperatorIterator = VectorOf<CLinterOperator*>::iterator;
using COperatorIterator = VectorOf<CLinterOperator*>::const_iterator;

using __CodePosition = std::tuple<size_t, size_t>;


template<class ContainerType>
class CLinter
{
	static_assert(std::is_same_v<ContainerType, CToken> || std::is_same_v<ContainerType, CPunctuationToken>, 
		VSL("ContainerType must have CToken as the base"));
public:
	CLinter() = delete;

	explicit CLinter(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end){}
	virtual ~CLinter() = default;

	[[nodiscard]] constexpr bool IsEndOfBuffer() const noexcept { return m_iterPos == m_iterEnd; }

	[[nodiscard]] constexpr auto GetIteratorSafe() { return IsEndOfBuffer() ? *std::prev(m_iterPos) : *m_iterPos; }

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
};

enum EvaluationType
{
	evaluate_everything,
	evaluate_singular
};
