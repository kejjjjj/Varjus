#pragma once

#include <utility>
#include <optional>

#include "linter/expressions/definitions.hpp"
#include "linter/expressions/expression_context.hpp"

#include "globalEnums.hpp"

class CMemory;
class CScope;
class AbstractSyntaxTree;
class IPostfixBase;
struct CKeyValue;

using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;

enum EOperandBaseType : char {
	ot_immediate,
	ot_identifier,
	ot_abstract_syntax_tree,
	ot_array,
	ot_object,
	ot_ternary,
	ot_lambda,
};

struct IOperand
{
	NONCOPYABLE(IOperand);

	IOperand() = default;
	virtual ~IOperand() = default;
	
	[[nodiscard]] virtual EOperandBaseType Type() const noexcept = 0;
	[[nodiscard]] virtual UniqueAST ToAST() = 0;
};

class CLinterOperand final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CLinterOperand);
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand(std::optional<PairMatcher>& eoe);
	[[nodiscard]] UniqueAST ToAST();

private:
	[[nodiscard]] std::unique_ptr<IOperand> ParseImmediate();
	[[nodiscard]] std::unique_ptr<IOperand> ParseParentheses();
	[[nodiscard]] std::unique_ptr<IOperand> ParseIdentifier();
	[[nodiscard]] std::unique_ptr<IOperand> ParseTernary(std::optional<PairMatcher>& eoe);

	[[nodiscard]] std::unique_ptr<IOperand> ParseArray();
	[[nodiscard]] std::unique_ptr<CKeyValue>ParseKeyValue(std::optional<PairMatcher> eoe);
	[[nodiscard]] std::unique_ptr<IOperand> ParseObject();
	[[nodiscard]] std::unique_ptr<IOperand> ParseLambda();

	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe, LinterIterator& pos) const noexcept;

	[[nodiscard]] UniqueAST PostfixesToAST() const noexcept;
	[[nodiscard]] UniqueAST OperandToAST() const noexcept;

	VectorOf<const CPunctuationToken*> m_oUnaryTokens;
	std::unique_ptr<IOperand> m_pOperand;
	VectorOf<std::unique_ptr<IPostfixBase>> m_oPostfixes;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;
};
