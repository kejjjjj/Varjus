#pragma once

#include <optional>

#include "definitions.hpp"
#include "expression_context.hpp"

class CLinterOperand;
class CLinterOperator;
class CMemory;
class CScope;
struct CExpressionList;

enum OperatorPriority : char;

/***********************************************************************
 > stores the operand on the lhs and optionally the operator (if exists)
 > throws on failure
***********************************************************************/
class CLinterSubExpression final : public CLinterSingle<CToken>
{
	VARJUS_NONCOPYABLE(CLinterSubExpression);
	friend class CLinterExpression;
public:

	CLinterSubExpression() = delete;
	explicit CLinterSubExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope,
		CMemory* const stack, std::optional<PairMatcher>& eoe);
	~CLinterSubExpression();

	/*
	 \return failure when end-of-expression is reached
	 \return keep calling this until failure is returned
	 */
	[[nodiscard]] Varjus::Success ParseSubExpression(std::optional<PairMatcher>& eoe, CExpressionList* expression, EvaluationType evalType);

private:

	[[nodiscard]] bool EndOfExpression() const noexcept;
	[[nodiscard]] OperatorPriority GetPriority() const noexcept;


	std::unique_ptr<CLinterOperand> m_oLhsOperand;
	std::unique_ptr<CLinterOperator> m_oOperator;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;

	std::optional<PairMatcher>& m_oEndOfExpression;
};