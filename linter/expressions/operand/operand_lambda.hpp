#pragma once

#include "operand.hpp"

#include "api/types/internal/references.hpp"
#include "runtime/structure.hpp"

class CRuntimeFunctionBase;
using ElementIndex = std::size_t;

struct CExpressionList;

struct CLambdaOperand final : public IOperand
{
	NONCOPYABLE(CLambdaOperand);

	CLambdaOperand() = default;
	CLambdaOperand(std::unique_ptr<CRuntimeFunctionBase>&& ptr, VectorOf<CCrossModuleReference>&& captures);
	~CLambdaOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_lambda;
	}

	[[nodiscard]] ASTNode ToAST() override;

	std::unique_ptr<CRuntimeFunctionBase> m_pLambda;
	VectorOf<CCrossModuleReference> m_oVariableCaptures;

};

//short lambda (=>) ambiguity checker class
//(args) => return 1;
//(args) => {}
class CLambdaChecker final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CLambdaChecker);
public:
	CLambdaChecker() = delete;
	explicit CLambdaChecker(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLambdaChecker();

	[[nodiscard]] Success Parse(
		std::optional<PairMatcher>& m_oEndOfExpression,
		CExpressionList* expression = nullptr,
		EvaluationType evalType = evaluate_everything);

	[[nodiscard]] inline std::unique_ptr<CLambdaOperand> Result() noexcept { return std::move(m_pOperand); }

private:

	[[nodiscard]] Success ParseInternal(
		std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression, EvaluationType evalType);

	[[nodiscard]] Success ParseParameters();

	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;

	[[nodiscard]] bool IsArrowFunction() const noexcept;
	[[nodiscard]] bool IsOneLiner() const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;
	VectorOf<std::string> m_oParameters;
	std::unique_ptr<CLambdaOperand> m_pOperand;
};
