#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorParser;

enum OperatorPriority : char;

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
	[[nodiscard]] OperatorPriority GetPriority() const noexcept;

	std::unique_ptr<CLinterOperand> m_oLhsOperand;
	std::unique_ptr<CLinterOperatorParser> m_oOperator;
	//std::unique_ptr<CLinterOperand> m_oRhsOperand;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};

struct CSortableSubExpression
{
	const CLinterOperand* m_oLhsOperand;
	const CLinterOperatorParser* m_oOperator;
	//const CLinterOperand* m_oRhsOperand;

};
