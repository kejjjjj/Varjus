#include "rtwhile.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include <ranges>
#include <algorithm>

#include <chrono>
#include <runtime/exceptions/exception.hpp>


CRuntimeWhileStatement::CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}
CRuntimeWhileStatement::~CRuntimeWhileStatement() = default;


IValue* CRuntimeWhileStatement::Execute([[maybe_unused]] CRuntimeContext* const ctx)
{

	while (true) {
		auto condition = m_pCondition->Evaluate(ctx);
		
		if (!condition->IsBooleanConvertible())
			throw CRuntimeError("the operand is not convertible to a boolean");

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