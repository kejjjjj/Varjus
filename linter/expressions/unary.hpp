#pragma once

#include "definitions.hpp"

class CUnaryBase;
class CMemory;
class AbstractSyntaxTree;

class CUnaryLinter final : public CVectorLinter<CPunctuationToken>
{
public:
	CUnaryLinter() = delete;
	explicit CUnaryLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CUnaryLinter();

	[[maybe_unused]] Varjus::Success ParseUnary();

	[[nodiscard]] VectorOf<std::unique_ptr<CUnaryBase>> Move() noexcept;
private:
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseNegation();
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseIncrement();
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseDecrement();
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseLogicalNot();
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseBitwiseNot();

	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseTypeOf();
	[[nodiscard]] std::unique_ptr<CUnaryBase> ParseToString();

	[[nodiscard]] bool IsUnaryOperator(const CPunctuationToken& token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<CUnaryBase>> m_oUnaryOperators;
};

enum EUnaryType{
	un_negation,
	un_increment,
	un_decrement,
	un_logical_not,
	un_bitwise_not,
	un_typeof,
	un_tostring,
};

class CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryBase);
public:
	CUnaryBase() = default;
	virtual ~CUnaryBase() = default;

	[[nodiscard]] virtual constexpr EUnaryType Type() const noexcept = 0;
	[[nodiscard]] virtual std::unique_ptr<AbstractSyntaxTree> ToAST() = 0;

	__CodePosition m_oCodePosition;
};
class CUnaryNegation final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryNegation);
public:
	CUnaryNegation() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_negation; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CUnaryIncrement final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryIncrement);
public:
	CUnaryIncrement() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_increment; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CUnaryDecrement final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryDecrement);
public:
	CUnaryDecrement() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_decrement; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CUnaryLogicalNot final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryLogicalNot);
public:
	CUnaryLogicalNot() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_logical_not; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CUnaryBitwiseNot final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryBitwiseNot);
public:
	CUnaryBitwiseNot() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_bitwise_not; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CUnaryTypeOf final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryTypeOf);
public:
	CUnaryTypeOf() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_typeof; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};

class CUnaryToString final : public CUnaryBase
{
	VARJUS_NONCOPYABLE(CUnaryToString);
public:
	CUnaryToString() = default;
	[[nodiscard]] constexpr EUnaryType Type() const noexcept override { return un_typeof; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};