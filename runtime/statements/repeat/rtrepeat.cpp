#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"

#include <algorithm>
#include <ranges>

using namespace Varjus;

CRuntimeRepeatStatement::CRuntimeRepeatStatement(ASTNode&& condition, __InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}
CRuntimeRepeatStatement::~CRuntimeRepeatStatement() = default;

static bool EvaluateCondition(Varjus::CRuntimeContext* const ctx, std::unique_ptr<CRuntimeExpression>& m_pCondition)
{
	auto condition = m_pCondition->Evaluate(ctx);

	if (!condition->IsBooleanConvertible())
		throw CRuntimeError(ctx->m_pRuntime, VSL("the operand is not convertible to a boolean"));

	const auto boolValue = condition->ToBoolean();

	if (!condition->HasOwner())
		condition->Release();

	return boolValue;
}

IValue* CRuntimeRepeatStatement::Execute([[maybe_unused]] Varjus::CRuntimeContext* const ctx)
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