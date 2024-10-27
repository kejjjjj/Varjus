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



class CLinterExpression final : protected IRuntimeBlock 
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterExpression();

	[[maybe_unused]] Success ParseExpression(std::optional<PairMatcher> m_oEndOfExpression=std::nullopt);
	[[nodiscard]] std::string ToString() const noexcept;

	std::unique_ptr<AbstractSyntaxTree> ToAST() const;

	RuntimeBlock ToRuntimeObject() const override;

private:
	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;

	//std::optional<PairMatcher> m_oEndOfExpression;
	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	WeakScope m_pScope;
	CMemory* const m_pOwner;
};


