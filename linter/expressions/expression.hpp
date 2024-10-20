#pragma once

#include <optional>

#include "definitions.hpp"
#include "expression_context.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;
class CLinterSubExpression;
class AbstractSyntaxTree;
class CMemoryData;
struct CSortedSubExpression;



class CLinterExpression
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	~CLinterExpression();

	[[maybe_unused]] Success ParseExpression(std::optional<PairMatcher> m_oEndOfExpression=std::nullopt);
	[[nodiscard]] std::string ToString() const noexcept;
	[[nodiscard]] std::string SortedToString() const noexcept;

	void QuickEvalAST();

	std::unique_ptr<AbstractSyntaxTree> ToAST() const;

private:
	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;
	int QuickEvalASTInternal(const AbstractSyntaxTree* node);

	//std::optional<PairMatcher> m_oEndOfExpression;
	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	CMemoryData* const m_pOwner;
};


