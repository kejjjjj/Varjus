#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/declarations/stack.hpp"

class CFunctionLinter
{
	NONCOPYABLE(CFunctionLinter);
public:
	explicit CFunctionLinter(LinterIterator& pos, LinterIterator& end, CMemoryData* const owner);

	Success ParseFunctionDeclaration();
	Success ParseFunctionParameters();

private:

	Success ParseFunctionParametersRecursively();

	[[nodiscard]] bool IsEndOfBuffer() const noexcept { return m_iterPos == m_iterEnd; }
	[[nodiscard]] bool IsFn(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	CMemoryData* const m_pOwner;

	std::string m_oFunctionName;
	VectorOf<std::string> m_oParameters;
};

class CRuntimeFunction
{

};
