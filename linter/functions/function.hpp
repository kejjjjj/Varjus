#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/declarations/stack.hpp"
#include "runtime/structure.hpp"

class CRuntimeStructure;
struct CFunctionBlock;

class CFunctionLinter final : protected IRuntimeBlock
{
	NONCOPYABLE(CFunctionLinter);
public:
	explicit CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

	[[nodiscard]] Success ParseFunction();
	[[nodiscard]] Success ParseFunctionParameters();

	[[nodiscard]] std::unique_ptr<CFunctionBlock> ToFunction() const;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
private:
	[[nodiscard]] Success ParseFunctionDeclaration();
	[[nodiscard]] Success ParseFunctionScope();

	[[nodiscard]] Success ParseFunctionParametersRecursively();

	[[nodiscard]] bool IsEndOfBuffer() const noexcept { return m_iterPos == m_iterEnd; }
	[[nodiscard]] bool IsFn(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	WeakScope m_pScope;
	CMemory* const m_pOwner;

	std::string m_oFunctionName;
	VectorOf<std::string> m_oParameters;

	std::unique_ptr<CStack> m_pThisStack;
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
	std::size_t m_uNesting{}; //current nesting offset from root scope
};
#pragma pack(pop)