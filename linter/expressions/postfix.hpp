#pragma once

#include "definitions.hpp"

#include "runtime/structure.hpp"

class AbstractSyntaxTree;
class IPostfixBase;
class CMemory;

class CPostfixLinter final : public CVectorLinter<CPunctuationToken>
{
public:
	CPostfixLinter() = delete;
	explicit CPostfixLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CPostfixLinter();

	[[maybe_unused]] Success ParsePostfix();
	[[nodiscard]] auto& GetTokens() const { return m_oTokens; }

	[[nodiscard]] VectorOf<std::unique_ptr<IPostfixBase>> Move() noexcept;
private:

	[[nodiscard]] std::unique_ptr<IPostfixBase> ParseMemberAccess();
	[[nodiscard]] std::unique_ptr<IPostfixBase> ParseSubscript();
	[[nodiscard]] std::unique_ptr<IPostfixBase> ParseFunctionCall();
	[[nodiscard]] std::unique_ptr<IPostfixBase> ParseIncrement();
	[[nodiscard]] std::unique_ptr<IPostfixBase> ParseDecrement();
	[[nodiscard]] bool IsPostfixOperator(const CPunctuationToken& token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<IPostfixBase>> m_oPostfixes;
};

enum EPostfixType
{
	pf_increment,
	pf_decrement,
	pf_subscript,
	pf_functioncall,
};

class IPostfixBase
{
	NONCOPYABLE(IPostfixBase);
public:
	IPostfixBase() = default;
	virtual ~IPostfixBase() = default;

	[[nodiscard]] virtual constexpr EPostfixType Type() const noexcept = 0;
	[[nodiscard]] virtual std::unique_ptr<AbstractSyntaxTree> ToAST() = 0;

	CodePosition m_oCodePosition;
};

class CPostfixMemberAccess final : public IPostfixBase
{
	NONCOPYABLE(CPostfixMemberAccess);
public:
	CPostfixMemberAccess(std::size_t globalMemberIndex)
		: m_uGlobalMemberIndex(globalMemberIndex) {}

	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_subscript; }
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;


private:
	std::size_t m_uGlobalMemberIndex;
};

class CPostfixSubscript final : public IPostfixBase
{
	NONCOPYABLE(CPostfixSubscript);
public:
	CPostfixSubscript(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CPostfixSubscript();

	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_subscript; }
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;


private:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class CPostfixFunctionCall final : public IPostfixBase
{
	NONCOPYABLE(CPostfixFunctionCall);
public:
	CPostfixFunctionCall();
	CPostfixFunctionCall(ExpressionList&& args);
	~CPostfixFunctionCall();

	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_functioncall; }
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;

private:
	ExpressionList m_pArgs;
};

class CPostfixIncrement final : public IPostfixBase
{
	NONCOPYABLE(CPostfixIncrement);
public:
	CPostfixIncrement() = default;
	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_increment; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
class CPostfixDecrement final : public IPostfixBase
{
	NONCOPYABLE(CPostfixDecrement);
public:
	CPostfixDecrement() = default;
	[[nodiscard]] constexpr EPostfixType Type() const noexcept override { return pf_decrement; }

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ToAST() override;
};
