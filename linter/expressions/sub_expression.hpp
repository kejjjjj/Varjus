#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorParser;


/***********************************************************************
 > stores the operand on the lhs and optionally the operator (if exists)
 > throws on failure
***********************************************************************/
class CLinterSubExpression
{
	NONCOPYABLE(CLinterSubExpression);
	friend class CLinterExpression;
public:

	CLinterSubExpression() = delete;
	explicit CLinterSubExpression(LinterIterator& pos, LinterIterator& end);
	~CLinterSubExpression();

	/*
	 \return failure when end-of-expression is reached
	 \return keep calling this until failure is returned
	 */
	[[nodiscard]] Success ParseSubExpression();

private:

	[[nodiscard]] bool EndOfExpression() const noexcept;

	std::unique_ptr<CLinterOperand> m_oLhsOperand;
	//std::unique_ptr<CLinterOperand> m_oRhsOperand;
	std::unique_ptr<CLinterOperatorParser> m_oOperator;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};