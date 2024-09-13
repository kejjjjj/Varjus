#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;
class CLinterSubExpression;

struct CLinterOperator;
struct CSortableSubExpression;

using OperatorIterator = std::vector<CLinterOperatorParser*>::iterator;

class CLinterExpression
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end);
	~CLinterExpression();

	[[nodiscard]] Success ParseExpression();
	[[nodiscard]] std::string ToString() const noexcept;
	[[nodiscard]] std::string SortedToString() const noexcept;

private:
	[[nodiscard]] bool EndOfExpression() const noexcept;

	void Sort();
	[[nodiscard]] std::vector<CSortableSubExpression> 
		SortIteratively(std::vector<CLinterOperand*>& operands, std::vector<CLinterOperatorParser*>& operators);

	[[nodiscard]] OperatorIterator FindHighestPriorityOperator(std::vector<CLinterOperatorParser*>& operators);

	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;
	std::vector<CSortableSubExpression> m_oSortedSubExpressions;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};


