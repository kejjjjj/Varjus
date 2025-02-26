#pragma once

#include <utility>
#include <optional>

#include "linter/expressions/definitions.hpp"
#include "linter/expressions/expression_context.hpp"

#include "varjus_api/internal/globalEnums.hpp"

class CMemory;
class CScope;
class AbstractSyntaxTree;
class IPostfixBase;
class CUnaryBase;
struct CKeyValue;

using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

enum EOperandBaseType : char {
	ot_immediate,
	ot_identifier,
	ot_abstract_syntax_tree,
	ot_array,
	ot_object,
	ot_ternary,
	ot_lambda,
	ot_fmt_string,
};

struct IOperand
{
	NONCOPYABLE(IOperand);

	IOperand() = default;
	virtual ~IOperand() = default;
	
	[[nodiscard]] virtual EOperandBaseType Type() const noexcept = 0;
	[[nodiscard]] virtual ASTNode ToAST() = 0;

	CodePosition m_oCodePosition;
};

class CLinterOperand final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CLinterOperand);
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand(std::optional<PairMatcher>& eoe);
	[[nodiscard]] ASTNode ToAST();


	[[nodiscard]] inline bool IsTernary() const noexcept { 
		return m_pOperand && m_pOperand->Type() == ot_ternary; }

private:
	[[nodiscard]] std::unique_ptr<IOperand> ParseImmediate();
	[[nodiscard]] std::unique_ptr<IOperand> ParseParentheses(std::optional<PairMatcher>&);
	[[nodiscard]] std::unique_ptr<IOperand> ParseIdentifier();
	[[nodiscard]] std::unique_ptr<IOperand> ParseTernary(std::optional<PairMatcher>& eoe);

	[[nodiscard]] std::unique_ptr<IOperand> ParseArray();
	[[nodiscard]] std::unique_ptr<CKeyValue>ParseKeyValue(std::optional<PairMatcher> eoe);
	[[nodiscard]] std::unique_ptr<IOperand> ParseObject();
	[[nodiscard]] std::unique_ptr<IOperand> ParseLambda();
	[[nodiscard]] std::unique_ptr<IOperand> ParseFormatString();

	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe, LinterIterator& pos) const noexcept;

	[[nodiscard]] ASTNode PostfixesToAST() const noexcept;
	[[nodiscard]] ASTNode UnariesToAST() const noexcept;

	[[nodiscard]] ASTNode OperandToAST() const noexcept;

	std::unique_ptr<IOperand> m_pOperand;
	VectorOf<std::unique_ptr<IPostfixBase>> m_oPostfixes;
	VectorOf<std::unique_ptr<CUnaryBase>> m_oUnaryOperators;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;
};
