#pragma once

#include "definitions.hpp"

class CUnaryBase;
class CMemory;
class AbstractSyntaxTree;
class CUnaryIncrement;

class CUnaryLinter final : public CVectorLinter<CPunctuationToken>
{
public:
	CUnaryLinter() = delete;
	explicit CUnaryLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CUnaryLinter();

	[[maybe_unused]] Success ParseUnary();

	[[nodiscard]] VectorOf<std::unique_ptr<CUnaryBase>> Move() noexcept;
private:

	[[nodiscard]] std::unique_ptr<CUnaryIncrement> ParseIncrement();

	[[nodiscard]] bool IsUnaryOperator(const CPunctuationToken& token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<CUnaryBase>> m_oUnaryOperators;
};

enum EUnaryType{
	un_increment,
};

class CUnaryBase
{
	NONCOPYABLE(CUnaryBase);
public:
	CUnaryBase() = default;
	virtual ~CUnaryBase() = default;

	[[nodiscard]] virtual constexpr EUnaryType Type() const noexcept = 0;
	[[nodiscard]] virtual std::unique_ptr<AbstractSyntaxTree> ToAST() = 0;

	CodePosition m_oCodePosition;
};

class CUnaryIncrement final : public CUnaryBase
{
	NONCOPYABLE(CUnaryIncrement);
public:
	CUnaryIncrement() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_increment; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
