#pragma once

#include <optional>

#include "definitions.hpp"
#include "expression_context.hpp"
#include "runtime/structure.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;
class CLinterSubExpression;
class AbstractSyntaxTree;
class CMemory;
class CScope;
struct CSortedSubExpression;

struct CExpressionList
{
	NONCOPYABLE(CExpressionList);
	CExpressionList();
	~CExpressionList();

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToMergedAST();
	[[nodiscard]] ExpressionList ToExpressionList();

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
	std::unique_ptr<CExpressionList> m_pNext;
};

class CLinterExpression final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterExpression();

	[[nodiscard]] Success Parse(std::optional<PairMatcher> m_oEndOfExpression=std::nullopt, CExpressionList* expression=nullptr);

	// merge all evaluated expressions into one
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToMergedAST() const;
	[[nodiscard]] ExpressionList ToExpressionList() const;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() const;

	[[maybe_unused]] Success ParseSequence(std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression);

	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;

	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;
	WeakScope m_pScope;
	CMemory* const m_pOwner;

	mutable std::unique_ptr<CExpressionList> m_pEvaluatedExpressions;
	//mutable std::unique_ptr<AbstractSyntaxTree> m_pNextExpression; // comma
};


