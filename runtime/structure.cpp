#include <algorithm>
#include <ranges>

#include "runtime/structure.hpp"
#include "linter/expressions/ast.hpp"
#include <cassert>

IRuntimeStructure::IRuntimeStructure() = default;
IRuntimeStructure::~IRuntimeStructure() = default;

IRuntimeStructureSequence::IRuntimeStructureSequence(InstructionSequence&& insns) 
	: m_oInstructions(std::move(insns)) {}
IRuntimeStructureSequence::~IRuntimeStructureSequence() = default;


CRuntimeConditionalStatement::CRuntimeConditionalStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pCondition(condition ? std::make_unique<CRuntimeExpression>(std::move(condition)) : nullptr) {}
CRuntimeConditionalStatement::~CRuntimeConditionalStatement() = default;



bool IRuntimeStructureSequence::ExecuteBlock([[maybe_unused]] CFunction* const thisFunction)
{
	for (auto& insn : m_oInstructions)
		if (insn->Execute(thisFunction))
			return true;

	return false;
}

CRuntimeFunction* CFileRuntimeData::FindFunction(const std::string& v) const
{
	const auto it = std::ranges::find(m_oFunctions, v, [](const RuntimeFunction& v) { return v->GetName(); });
	return it != m_oFunctions.end() ? it->get() : nullptr;
}
size_t CFileRuntimeData::FindFunctionIndex(const std::string& v) const
{
	const auto it = std::ranges::find(m_oFunctions, v, [](const RuntimeFunction& v) { return v->GetName(); });
	assert(it != m_oFunctions.end());
	return static_cast<std::size_t>(std::distance(m_oFunctions.begin(), it));
}
