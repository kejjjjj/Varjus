#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"

#include <algorithm>
#include <ranges>
using namespace Varjus;

CRuntimeWhileStatement::CRuntimeWhileStatement(ASTNode&& condition, __InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}
CRuntimeWhileStatement::~CRuntimeWhileStatement() = default;


IValue* CRuntimeWhileStatement::Execute([[maybe_unused]] Varjus::CRuntimeContext* const ctx)
{

	while (true) {
		auto condition = m_pCondition->Evaluate(ctx);
		
		if (!condition->IsBooleanConvertible())
			throw CRuntimeError(ctx->m_pRuntime, VSL("the operand is not convertible to a boolean"));

		const auto boolValue = condition->ToBoolean();

		if (!condition->HasOwner())
			condition->Release();

		if (!boolValue) {
			break;
		}

		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);
			if (lc == lc_break)
				break;
			if (lc == lc_continue)
				continue;

			return v;
		}
	}

	return nullptr;
}