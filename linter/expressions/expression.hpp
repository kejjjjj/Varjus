#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;

struct CLinterOperator;


class CLinterExpression
{
public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end);
	~CLinterExpression();

	[[nodiscard]] Success ParseExpression();

private:

	UniquePointerVector<CLinterOperand> m_oOperands;
	UniquePointerVector<CLinterOperatorParser> m_oOperators;

	std::vector<CLinterOperator> m_oOperatorLinks;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};

struct CLinterOperator
{
	const CLinterOperatorParser* m_pOperator = 0;
	const CLinterOperand* m_oLhsOperand = 0;
	const CLinterOperand* m_oRhsOperand = 0;
};
