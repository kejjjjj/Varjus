#include "linter/expressions/ast.hpp"
#include "runtime/structure.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

IRuntimeStructure::IRuntimeStructure() = default;
IRuntimeStructure::~IRuntimeStructure() = default;

IRuntimeStructureSequence::IRuntimeStructureSequence(InstructionSequence&& insns) 
	: m_oInstructions(std::move(insns)) {}
IRuntimeStructureSequence::~IRuntimeStructureSequence() = default;


CRuntimeConditionalStatement::CRuntimeConditionalStatement(ASTNode&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pCondition(condition ? std::make_unique<CRuntimeExpression>(std::move(condition)) : nullptr) {}
CRuntimeConditionalStatement::~CRuntimeConditionalStatement() = default;



IValue* IRuntimeStructureSequence::ExecuteBlock(CRuntimeContext* const ctx)
{
	for (auto& insn : m_oInstructions)
		if (auto v = insn->Execute(ctx))
			return v;

	return nullptr;
}


EExecutionControl IRuntimeStructure::ToControlStatement(const IValue* rv) {

	const auto v = reinterpret_cast<std::size_t>(rv);

	if (v > static_cast<std::size_t>(lc_continue))
		return lc_null;

	return static_cast<EExecutionControl>(v);
}