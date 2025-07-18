#pragma once

#include <optional>

#include "definitions.hpp"
#include "expression_context.hpp"
#include "varjus_api/internal/structure.hpp"

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
	VARJUS_NONCOPYABLE(CExpressionList);
	CExpressionList();
	~CExpressionList();

	[[nodiscard]] ASTNode ToMergedAST();
	[[nodiscard]] __ExpressionList ToExpressionList();

	ASTNode m_pAST;
	std::unique_ptr<CExpressionList> m_pNext;
};

class CLinterExpression final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterExpression();

	[[nodiscard]] Varjus::Success Parse(
		std::optional<PairMatcher> m_oEndOfExpression=std::nullopt, 
		CExpressionList* expression=nullptr,
		EvaluationType evalType = evaluate_everything);
	[[nodiscard]] Varjus::Success ParseInternal(
		std::optional<PairMatcher>& m_oEndOfExpression, 
		CExpressionList* expression = nullptr,
		EvaluationType evalType = evaluate_everything);

	// merge all evaluated expressions into one
	[[nodiscard]] ASTNode ToMergedAST() const;
	[[nodiscard]] __ExpressionList ToExpressionList() const;

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;

	//note: doesn't check context
	[[nodiscard]] static bool CanBeDiscarded(const ASTNode& node) noexcept;
private:
	[[nodiscard]] ASTNode ToAST() const;

	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;


	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;
	WeakScope m_pScope;
	CMemory* const m_pOwner;

	mutable std::unique_ptr<CExpressionList> m_pEvaluatedExpressions;
	//mutable std::unique_ptr<AbstractSyntaxTree> m_pNextExpression; // comma
};


