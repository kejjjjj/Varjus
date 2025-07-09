#pragma once

#include "linter/token.hpp"
#include "linter/statements/statement.hpp"
#include "varjus_api/internal/structure.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CLoopControlStatement : public CStatementLinter, protected IRuntimeBlock 
{
	VARJUS_NONCOPYABLE(CLoopControlStatement);
public:
	explicit CLoopControlStatement(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CLoopControlStatement();

	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;
private:

	EExecutionControl m_eType{lc_null};
};
