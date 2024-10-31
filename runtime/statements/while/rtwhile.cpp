#include "rtwhile.hpp"
#include "runtime/expressions/rtexpression.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include <ranges>
#include <algorithm>

#include <chrono>


CRuntimeWhileStatement::CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}
CRuntimeWhileStatement::~CRuntimeWhileStatement() = default;


bool CRuntimeWhileStatement::Execute([[maybe_unused]] CFunction* const thisFunction)
{

	while (true) {
		auto condition = m_pCondition->Evaluate(thisFunction);
		const auto boolValue = condition->ToBoolean();

		if (!condition->HasOwner())
			condition->Release();

		if (!boolValue) {
			break;
		}

		if (ExecuteBlock(thisFunction))
			return true;
	}

	return false;
}