#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;
class CLinterSubExpression;

struct CLinterOperator;


class CLinterExpression
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end);
	~CLinterExpression();

	[[nodiscard]] Success ParseExpression();

private:
	[[nodiscard]] bool EndOfExpression() const noexcept;


	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};