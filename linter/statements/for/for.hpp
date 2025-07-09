#pragma once

#include "linter/statements/statement.hpp"

#include <variant>

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

struct StandardForLoop
{
	ASTNode m_pInitializer;
	ASTNode m_pCondition;
	ASTNode m_pOnEnd;
};

struct RangedForLoop
{
	std::shared_ptr<class VariableASTNode> m_pIterator;
	ASTNode m_pIterable;
};

enum ForLoopType { for_standard, for_ranged };

class CForStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CForStatementLinter);

public:
	explicit CForStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CForStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;

private:

	[[nodiscard]] Varjus::Success ParseInitializer();
	[[nodiscard]] Varjus::Success ParseCondition();
	[[nodiscard]] Varjus::Success ParseEndExpression();

	[[nodiscard]] Varjus::Success ParseRangedForLoop();

	std::variant<StandardForLoop, RangedForLoop> m_oData;
	ForLoopType m_eType{};
};