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



IValue* IRuntimeStructureSequence::ExecuteBlock([[maybe_unused]] CFunction* const thisFunction)
{
	for (auto& insn : m_oInstructions)
		if (auto v = insn->Execute(thisFunction))
			return v;

	return nullptr;
}
void CFileRuntimeData::AddGlobalInstructions(VectorOf<RuntimeBlock>&& insns){
	m_oGlobalScopeInstructions = std::move(insns);
}
CRuntimeFunction* CFileRuntimeData::FindFunction(const std::string& v) const
{
	const auto it = std::ranges::find(m_oFunctions, v, [](const RuntimeFunction& v) { return v->GetName(); });
	return it != m_oFunctions.end() ? it->get() : nullptr;
}

EExecutionControl IRuntimeStructure::ToControlStatement(const IValue* rv) {

	const auto v = reinterpret_cast<std::size_t>(rv);

	if (v > static_cast<std::size_t>(lc_continue))
		return lc_null;

	return static_cast<EExecutionControl>(v);
}