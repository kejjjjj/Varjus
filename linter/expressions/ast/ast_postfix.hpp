#pragma once
#include "ast_main.hpp"

class PostfixASTNode : public OperatorASTNode
{
public:
	PostfixASTNode() = default;
	PostfixASTNode(const CodePosition& pos) : OperatorASTNode(pos) {}
	PostfixASTNode(const CodePosition& pos, Punctuation punc) : OperatorASTNode(pos,punc) {}
	~PostfixASTNode() = default;

	[[nodiscard]] constexpr const PostfixASTNode* GetPostfix() const noexcept override { return this; }

	[[nodiscard]] constexpr bool IsPostfix() const noexcept override { return true; }

	[[nodiscard]] virtual constexpr bool IsMemberAccess() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSubscript() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunctionCall() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsIncrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsDecrement() const noexcept { return false; }

};

class MemberAccessASTNode : public PostfixASTNode
{
	NONCOPYABLE(MemberAccessASTNode);
public:
	MemberAccessASTNode(const CodePosition& pos, std::size_t globalMemberIndex)
		: PostfixASTNode(pos), m_uGlobalMemberIndex(globalMemberIndex) {
	}

	[[nodiscard]] constexpr bool IsMemberAccess() const noexcept override { return true; }
	std::size_t m_uGlobalMemberIndex;
};

class SubscriptASTNode : public PostfixASTNode
{
	NONCOPYABLE(SubscriptASTNode);
public:
	SubscriptASTNode(const CodePosition& pos, std::unique_ptr<AbstractSyntaxTree>&& expression)
		: PostfixASTNode(pos), m_pAST(std::move(expression)) {
	}
	
	[[nodiscard]] constexpr bool IsSubscript() const noexcept override { return true; }

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class FunctionCallASTNode : public PostfixASTNode
{
	NONCOPYABLE(FunctionCallASTNode);
public:
	FunctionCallASTNode(const CodePosition& pos, VectorOf<std::unique_ptr<AbstractSyntaxTree>>&& args)
		: PostfixASTNode(pos), m_oArguments(std::move(args)) {
	}
	[[nodiscard]] constexpr bool IsFunctionCall() const noexcept override { return true; }

	VectorOf<std::unique_ptr<AbstractSyntaxTree>> m_oArguments;
};

class PostfixIncrementAST : public PostfixASTNode
{
	NONCOPYABLE(PostfixIncrementAST);
public:
	PostfixIncrementAST(const CodePosition& pos)
		: PostfixASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsIncrement() const noexcept override { return true; }
};
class PostfixDecrementAST : public PostfixASTNode
{
	NONCOPYABLE(PostfixDecrementAST);
public:
	PostfixDecrementAST(const CodePosition& pos)
		: PostfixASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsDecrement() const noexcept override { return true; }
};