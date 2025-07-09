#pragma once
#include "ast_main.hpp"

class SubscriptASTNode;
class MemberAccessASTNode;
class FunctionCallASTNode;

class PostfixASTNode : public OperatorASTNode
{
public:
	PostfixASTNode() = default;
	PostfixASTNode(const __CodePosition& pos) : OperatorASTNode(pos) {}
	PostfixASTNode(const __CodePosition& pos, Punctuation punc) : OperatorASTNode(pos,punc) {}
	~PostfixASTNode() = default;

	[[nodiscard]] constexpr const PostfixASTNode* GetPostfix() const noexcept override { return this; }
	[[nodiscard]] constexpr PostfixASTNode* GetPostfix() noexcept override { return this; }

	[[nodiscard]] constexpr bool IsPostfix() const noexcept override { return true; }

	[[nodiscard]] virtual constexpr bool IsMemberAccess() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSubscript() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunctionCall() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsIncrement() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsDecrement() const noexcept { return false; }

	[[nodiscard]] virtual constexpr const SubscriptASTNode* GetSubscript() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr SubscriptASTNode* GetSubscript() noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const MemberAccessASTNode* GetMemberAccess() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const FunctionCallASTNode* GetFunctionCall() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr FunctionCallASTNode* GetFunctionCall() noexcept { return nullptr; }

	[[nodiscard]] virtual bool IsMeaningful() const noexcept override { return false; }

};

class MemberAccessASTNode : public PostfixASTNode
{
	VARJUS_NONCOPYABLE(MemberAccessASTNode);
public:
	MemberAccessASTNode(const __CodePosition& pos, std::size_t globalMemberIndex)
		: PostfixASTNode(pos), m_uGlobalMemberIndex(globalMemberIndex) {
	}

	[[nodiscard]] constexpr bool IsMemberAccess() const noexcept override { return true; }
	[[nodiscard]] constexpr const MemberAccessASTNode* GetMemberAccess() const noexcept override { return this; }

	std::size_t m_uGlobalMemberIndex;
};

class SubscriptASTNode : public PostfixASTNode
{
	VARJUS_NONCOPYABLE(SubscriptASTNode);
public:
	SubscriptASTNode(const __CodePosition& pos, ASTNode&& expression)
		: PostfixASTNode(pos), m_pAST(std::move(expression)) {
	}
	
	[[nodiscard]] constexpr bool IsSubscript() const noexcept override { return true; }
	[[nodiscard]] constexpr const SubscriptASTNode* GetSubscript() const noexcept override { return this; }
	[[nodiscard]] constexpr SubscriptASTNode* GetSubscript() noexcept override { return this; }

	ASTNode m_pAST;
};

class FunctionCallASTNode : public PostfixASTNode
{
	VARJUS_NONCOPYABLE(FunctionCallASTNode);
public:
	FunctionCallASTNode(const __CodePosition& pos, VectorOf<ASTNode>&& args)
		: PostfixASTNode(pos), m_oArguments(std::move(args)) {
	}
	[[nodiscard]] constexpr bool IsFunctionCall() const noexcept override { return true; }
	[[nodiscard]] constexpr const FunctionCallASTNode* GetFunctionCall() const noexcept override { return this; }
	[[nodiscard]] constexpr FunctionCallASTNode* GetFunctionCall() noexcept override { return this; }
	[[nodiscard]] bool IsMeaningful() const noexcept override { return true; }

	VectorOf<ASTNode> m_oArguments;
};

class PostfixIncrementAST : public PostfixASTNode
{
	VARJUS_NONCOPYABLE(PostfixIncrementAST);
public:
	PostfixIncrementAST(const __CodePosition& pos)
		: PostfixASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsIncrement() const noexcept override { return true; }
	[[nodiscard]] bool IsMeaningful() const noexcept override { return true; }

};
class PostfixDecrementAST : public PostfixASTNode
{
	VARJUS_NONCOPYABLE(PostfixDecrementAST);
public:
	PostfixDecrementAST(const __CodePosition& pos)
		: PostfixASTNode(pos) {
	}

	[[nodiscard]] constexpr bool IsDecrement() const noexcept override { return true; }
	[[nodiscard]] bool IsMeaningful() const noexcept override { return true; }

};