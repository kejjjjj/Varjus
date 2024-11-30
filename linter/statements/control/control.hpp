#pragma once

#include "linter/token.hpp"
#include "linter/statements/statement.hpp"
#include "runtime/structure.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CLoopControlStatement : public CStatementLinter, protected IRuntimeBlock 
{
	NONCOPYABLE(CLoopControlStatement);
public:
	explicit CLoopControlStatement(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CLoopControlStatement();

	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
private:

	ELoopControl m_eType{lc_null};
};
