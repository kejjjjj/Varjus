#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/functions/stack.hpp"
#include "varjus_api/internal/structure.hpp"

class IRuntimeStructure;
struct CFunctionBlock;

struct CParameterData
{
	VarjusString m_sName;
	CodePosition m_oPosition;
};

class CFunctionLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	NONCOPYABLE(CFunctionLinter);
	friend class CLinterOperand;
	friend class CLambdaChecker;

public:
	explicit CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CFunctionLinter();
	[[nodiscard]] Success Parse();
	[[nodiscard]] Success ParseFunctionParameters();
	[[nodiscard]] Success ParseFunctionScope();

	[[nodiscard]] std::unique_ptr<CFunctionBlock> ToFunction() const;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] std::unique_ptr<CRuntimeFunction> ToRuntimeFunction() const;

	[[nodiscard]] VectorOf<CCrossModuleReference> GetParameterIndices(CStack* stack) const;
	[[nodiscard]] VectorOf<CCrossModuleReference> GetVariableIndices(CStack* stack) const;
	[[nodiscard]] VectorOf<CCrossModuleReference> GetSharedOwnershipVariables(CStack* stack) const;

	[[nodiscard]] constexpr auto& GetName() const noexcept { return m_oFunctionName; }
	[[nodiscard]] static bool IsIdentifier(const CToken* token) noexcept;

private:
	[[nodiscard]] Success ParseFunctionDeclaration();

	[[nodiscard]] Success ParseFunctionParametersRecursively();


	[[nodiscard]] bool IsFn(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VarjusString m_oFunctionName;
	VectorOf<VarjusString> m_oParameters;

	std::unique_ptr<CStack> m_pThisStack;
	std::shared_ptr<CScope> m_pThisScope;
};

#pragma pack(push)
WARNING_DISABLE(4623) // default constructor implicitly deleted
WARNING_DISABLE(5027) // move assignment operator implicitly deleted
struct CFunctionBlock final
{
	VarjusString m_sName; // function name
	std::size_t m_uNumParameters; // function paramters
	CStack* const m_pStack; // the stack
	VectorOf<RuntimeBlock> m_oInstructions; // sorted list of all function instructions
};
#pragma pack(pop)