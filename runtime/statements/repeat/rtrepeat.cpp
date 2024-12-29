#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/exceptions/exception.hpp"

#include <ranges>
#include <algorithm>


CRuntimeRepeatStatement::CRuntimeRepeatStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}
CRuntimeRepeatStatement::~CRuntimeRepeatStatement() = default;

static bool EvaluateCondition(CRuntimeContext* const ctx, std::unique_ptr<CRuntimeExpression>& m_pCondition)
{
	auto condition = m_pCondition->Evaluate(ctx);

	if (!condition->IsBooleanConvertible())
		throw CRuntimeError("the operand is not convertible to a boolean");

	const auto boolValue = condition->ToBoolean();

	if (!condition->HasOwner())
		condition->Release();

	return boolValue;
}

IValue* CRuntimeRepeatStatement::Execute([[maybe_unused]] CRuntimeContext* const ctx)
{

	const auto hasAST = m_pCondition->HasAST();

	while (true) {


		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);
			if (lc == lc_break) {
				if (!hasAST || EvaluateCondition(ctx, m_pCondition))
					break;
			} else if (lc == lc_continue) {
				if (!hasAST || EvaluateCondition(ctx, m_pCondition))
					continue;
			}

			return v;
		}

		if (!hasAST) //loop forever when the condition isn't set
			continue;
		
		if (!EvaluateCondition(ctx, m_pCondition))
			break;
	}

	return nullptr;
}