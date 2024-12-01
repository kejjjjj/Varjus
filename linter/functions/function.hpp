#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/functions/stack.hpp"
#include "runtime/structure.hpp"

class IRuntimeStructure;
struct CFunctionBlock;

class CFunctionLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	NONCOPYABLE(CFunctionLinter);
	friend class CLinterOperand;
public:
	explicit CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

	[[nodiscard]] Success Parse();
	[[nodiscard]] Success ParseFunctionParameters();
	[[nodiscard]] Success ParseFunctionScope();

	[[nodiscard]] std::unique_ptr<CFunctionBlock> ToFunction() const;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] std::unique_ptr<CRuntimeFunction> ToRuntimeFunction() const;

	[[nodiscard]] VectorOf<std::size_t> GetParameterIndices(CStack* stack) const;
	[[nodiscard]] VectorOf<std::size_t> GetVariableIndices(CStack* stack) const;
	[[nodiscard]] VectorOf<std::size_t> GetSharedOwnershipVariables(CStack* stack) const;

private:
	[[nodiscard]] Success ParseFunctionDeclaration();

	[[nodiscard]] Success ParseFunctionParametersRecursively();


	[[nodiscard]] bool IsFn(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	std::string m_oFunctionName;
	VectorOf<std::string> m_oParameters;

	std::unique_ptr<CStack> m_pThisStack;
	std::shared_ptr<CScope> m_pThisScope;
};

#pragma pack(push)
#pragma warning(disable : 4623) // default constructor implicitly deleted
#pragma warning(disable : 5027) // move assignment operator implicitly deleted
struct CFunctionBlock final
{
	std::string m_sName; // function name
	std::size_t m_uNumParameters; // function paramters
	CStack* const m_pStack; // the stack
	VectorOf<RuntimeBlock> m_oInstructions; // sorted list of all function instructions
};
#pragma pack(pop)