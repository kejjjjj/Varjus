#pragma once

#include <utility>
#include <optional>
#include "definitions.hpp"

#include "linter/expressions/expression_context.hpp"

class CMemory;
class CIdentifierLinter;
class CScope;

struct CMemoryIdentifier;
class AbstractSyntaxTree;

struct COperandBase;
struct CIdentifierOperand; 
struct CASTOperand;
class IPostfixBase;

struct CLinterFunction;
struct CLinterVariable;


using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
using ExpressionList = VectorOf<UniqueAST>;

enum EOperandBaseType : char {
	ot_identifier,
	ot_abstract_syntax_tree,
	ot_array,
	ot_key_value,
	ot_object
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
		return ot_identifier;
	}

	std::unique_ptr<CIdentifierLinter> m_oIdentifierToken;
};

struct CASTOperand final : public COperandBase
{
	NONCOPYABLE(CASTOperand);
	CASTOperand(UniqueAST&& ptr);
	~CASTOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_abstract_syntax_tree;
	}

	UniqueAST m_pAST;
};

struct CArrayOperand final : public COperandBase
{
	NONCOPYABLE(CArrayOperand);

	CArrayOperand() = default;
	CArrayOperand(ExpressionList&& ptr);
	~CArrayOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_array;
	}

	ExpressionList m_oExpressions;
};

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

struct CKeyValueOperand final : public COperandBase
{
	NONCOPYABLE(CKeyValueOperand);

	CKeyValueOperand() = default;
	CKeyValueOperand(KeyValue<std::size_t, UniqueAST>&& ptr);
	~CKeyValueOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_object;
	}

	KeyValue<std::size_t, UniqueAST> m_oValue;
};

struct CObjectOperand final : public COperandBase
{
	NONCOPYABLE(CObjectOperand);

	CObjectOperand() = default;
	CObjectOperand(VectorOf<KeyValue<std::size_t, UniqueAST>>&& ptr);
	~CObjectOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_object;
	}

	VectorOf<KeyValue<std::size_t, UniqueAST>> m_oAttributes;
};

class CLinterOperand final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CLinterOperand);
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, 
		CMemory* const stack, std::optional<PairMatcher>& eoe);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand();

	[[nodiscard]] UniqueAST ToAST();

	[[nodiscard]] bool IsExpression() const noexcept;
	[[nodiscard]] bool IsImmediate() const noexcept;

	[[nodiscard]] bool IsArray() const noexcept;
	[[nodiscard]] CArrayOperand* GetArray() const noexcept;

	[[nodiscard]] bool IsObject() const noexcept;
	[[nodiscard]] CObjectOperand* GetObject() const noexcept;

	[[nodiscard]] bool IsVariable() const noexcept;
	[[nodiscard]] const CLinterVariable* GetVariable() const noexcept;

	[[nodiscard]] bool IsFunction() const noexcept;
	[[nodiscard]] const CLinterFunction* GetFunction() const noexcept;

private:
	[[nodiscard]] std::unique_ptr<COperandBase> ParseParentheses();
	[[nodiscard]] std::unique_ptr<COperandBase> ParseIdentifier();
	[[nodiscard]] std::unique_ptr<COperandBase> ParseArray();
	[[nodiscard]] std::unique_ptr<COperandBase> ParseKeyValue();
	[[nodiscard]] std::unique_ptr<COperandBase> ParseObject();

	[[nodiscard]] UniqueAST PostfixesToAST() const noexcept;
	[[nodiscard]] UniqueAST OperandToAST() const noexcept;
	[[nodiscard]] UniqueAST ExpressionToAST() const noexcept;

	VectorOf<const CPunctuationToken*> m_oUnaryTokens;
	std::unique_ptr<COperandBase> m_pOperand;
	VectorOf<std::unique_ptr<IPostfixBase>> m_oPostfixes;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;
	std::optional<PairMatcher>& m_oEndOfExpression;
};
