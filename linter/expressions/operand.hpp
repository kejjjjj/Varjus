#pragma once

#include "definitions.hpp"

#include <iostream>

class CMemory;
class CIdentifierLinter;
class CScope;

struct CLinterVariable;
class AbstractSyntaxTree;

struct COperandBase;
struct CIdentifierOperand; 
struct CASTOperand;
class IPostfixBase;

enum EOperandBaseType : char {
	identifier,
	abstract_syntax_tree,
};

struct COperandBase
{
	NONCOPYABLE(COperandBase);

	COperandBase() = default;
	virtual ~COperandBase() = default;

	[[nodiscard]] virtual EOperandBaseType Type() const noexcept = 0;

	[[nodiscard]] CIdentifierLinter* GetIdentifier() const noexcept;
};

struct CIdentifierOperand final : public COperandBase
{
	NONCOPYABLE(CIdentifierOperand);
	CIdentifierOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) :
		m_oIdentifierToken(std::make_unique<CIdentifierLinter>(pos, end, scope, stack)){
	}
	~CIdentifierOperand() = default;

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return identifier;
	}

	std::unique_ptr<CIdentifierLinter> m_oIdentifierToken;
};

struct CASTOperand final : public COperandBase
{
	NONCOPYABLE(CASTOperand);
	CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr);
	~CASTOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return abstract_syntax_tree;
	}

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class CLinterOperand final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CLinterOperand);
	friend class AbstractSyntaxTree;
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand();
	
	[[nodiscard]] std::string ToString() const noexcept;

	[[nodiscard]] bool IsExpression() const noexcept;


	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST();

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> PostfixesToAST() const noexcept;


	[[nodiscard]] bool IsVariable() const noexcept;
	[[nodiscard]] bool IsImmediate() const noexcept;

	[[nodiscard]] const CLinterVariable* GetVariable() const noexcept;

	[[nodiscard]] const COperandBase* GetOperand() const noexcept { return m_pOperand.get(); }
	[[nodiscard]] std::unique_ptr<COperandBase>& GetOperandRaw() noexcept { return m_pOperand; }

private:

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> OperandToAST() const noexcept;
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ExpressionToAST() const noexcept;

	VectorOf<const CPunctuationToken*> m_oUnaryTokens;
	std::unique_ptr<COperandBase> m_pOperand;
	VectorOf<std::unique_ptr<IPostfixBase>> m_oPostfixes;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;
};
