#pragma once

#include "definitions.hpp"

class AbstractSyntaxTree;
class CPostfixBase;
class CPostfixSubscript;
class CMemory;

class CPostfixLinter final : public CVectorLinter<CPunctuationToken>
{
public:
	CPostfixLinter() = delete;
	explicit CPostfixLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CPostfixLinter();

	[[maybe_unused]] Success ParsePostfix();
	[[nodiscard]] auto& GetTokens() const { return m_oTokens; }

	[[nodiscard]] VectorOf<std::unique_ptr<CPostfixBase>> Move() noexcept;
private:

	[[nodiscard]] std::unique_ptr<CPostfixSubscript> ParseSubscript();
	[[nodiscard]] bool IsPostfixOperator(const CPunctuationToken& token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<CPostfixBase>> m_oPostfixes;
};

enum EPostfixType
{
	pf_increment,
	pf_decrement,
	pf_subscript,
};

class CPostfixBase
{
	NONCOPYABLE(CPostfixBase);
public:
	CPostfixBase() = default;
	virtual ~CPostfixBase() = default;

	[[nodiscard]] virtual std::unique_ptr<AbstractSyntaxTree> ToAST() = 0;
	[[nodiscard]] virtual constexpr EPostfixType Type() const noexcept = 0;
};

class CPostfixSubscript final : public CPostfixBase
{
	NONCOPYABLE(CPostfixSubscript);
public:
	CPostfixSubscript(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CPostfixSubscript();

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_subscript;}


private:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};
