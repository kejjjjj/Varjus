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

	[[nodiscard]] virtual constexpr bool IsIncrement() const noexcept { return false; }
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
