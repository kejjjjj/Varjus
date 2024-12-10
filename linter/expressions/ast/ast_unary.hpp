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
	[[nodiscard]] constexpr bool IsUnary() const noexcept override { return true; }

	[[nodiscard]] virtual constexpr bool IsNegation() const noexcept  { return false; }
	[[nodiscard]] virtual constexpr bool IsIncrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsDecrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsLogicalNot() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsTypeOf() const noexcept { return false; }

};
class UnaryNegationAST : public UnaryASTNode
{
	NONCOPYABLE(UnaryNegationAST);
public:
	UnaryNegationAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsNegation() const noexcept override { return true; }
};

class UnaryIncrementAST : public UnaryASTNode
{
	NONCOPYABLE(UnaryIncrementAST);
public:
	UnaryIncrementAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsIncrement() const noexcept override { return true; }
};
class UnaryDecrementAST : public UnaryASTNode
{
	NONCOPYABLE(UnaryDecrementAST);
public:
	UnaryDecrementAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsDecrement() const noexcept override { return true; }
};
class UnaryLogicalNotAST : public UnaryASTNode
{
	NONCOPYABLE(UnaryLogicalNotAST);
public:
	UnaryLogicalNotAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsLogicalNot() const noexcept override { return true; }
};

class UnaryTypeOfAST : public UnaryASTNode
{
	NONCOPYABLE(UnaryTypeOfAST);
public:
	UnaryTypeOfAST(const CodePosition& pos)
		: UnaryASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsTypeOf() const noexcept override { return true; }
};