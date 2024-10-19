#pragma once

#include "definitions.hpp"

class CLinterOperand;
class CLinterOperatorLinkage;
class CLinterOperatorParser;
class CLinterSubExpression;

struct CSortedSubExpression;

class AbstractSyntaxTree;

class CMemoryData;

class CCodeStructure;

class CLinterExpression
{
	NONCOPYABLE(CLinterExpression);

public:

	CLinterExpression() = delete;
	explicit CLinterExpression(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	~CLinterExpression();

	[[maybe_unused]] Success ParseExpression();
	[[nodiscard]] std::string ToString() const noexcept;
	[[nodiscard]] std::string SortedToString() const noexcept;

	void QuickEvalAST();

	std::shared_ptr<AbstractSyntaxTree> ToAST() const;

private:
	[[nodiscard]] bool EndOfExpression() const noexcept;

	int QuickEvalASTInternal(const AbstractSyntaxTree* node);


	/*void Sort();*/

	UniquePointerVector<CLinterSubExpression> m_oSubExpressions;
	//std::unique_ptr<AbstractSyntaxTree> m_pAST;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	CMemoryData* const m_pOwner;
};


