#pragma once
#include "ast_main.hpp"

class UnaryASTNode : public OperatorASTNode
{
public:
	UnaryASTNode() = default;
	UnaryASTNode(const CodePosition& pos) : OperatorASTNode(pos) {}
	UnaryASTNode(const CodePosition& pos, Punctuation punc) : OperatorASTNode(pos, punc) {}
	~UnaryASTNode() = default;

	[[nodiscard]] constexpr const UnaryASTNode* GetUnary() const noexcept override { return this; }
	[[nodiscard]] constexpr UnaryASTNode* GetUnary() noexcept override { return this; }

	[[nodiscard]] constexpr bool IsUnary() const noexcept override { return true; }

	[[nodiscard]] virtual constexpr bool IsNegation() const noexcept  { return false; }
	[[nodiscard]] virtual constexpr bool IsIncrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsDecrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsLogicalNot() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsBitwiseNot() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsTypeOf() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsToString() const noexcept { return false; }

	[[nodiscard]] virtual bool IsMeaningful() const noexcept override { return false; }
};
class UnaryNegationAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryNegationAST);
public:
	UnaryNegationAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsNegation() const noexcept override { return true; }
};

class UnaryIncrementAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryIncrementAST);
public:
	UnaryIncrementAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsIncrement() const noexcept override { return true; }
	[[nodiscard]] bool IsMeaningful() const noexcept override { return true; }

};
class UnaryDecrementAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryDecrementAST);
public:
	UnaryDecrementAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsDecrement() const noexcept override { return true; }
	[[nodiscard]] bool IsMeaningful() const noexcept override { return true; }

};
class UnaryLogicalNotAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryLogicalNotAST);
public:
	UnaryLogicalNotAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsLogicalNot() const noexcept override { return true; }
};
class UnaryBitwiseNotAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryBitwiseNotAST);
public:
	UnaryBitwiseNotAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsBitwiseNot() const noexcept override { return true; }
};

class UnaryTypeOfAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryTypeOfAST);
public:
	UnaryTypeOfAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsTypeOf() const noexcept override { return true; }
};

class UnaryToStringAST : public UnaryASTNode
{
	VARJUS_NONCOPYABLE(UnaryToStringAST);
public:
	UnaryToStringAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsToString() const noexcept override { return true; }
};
