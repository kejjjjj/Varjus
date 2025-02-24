#include "api/types/types.hpp"
#include "runtime/exceptions/exception.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/structure.hpp"
#include "api/internal/variables.hpp"


CRuntimeConditionalStatement* CRuntimeConditionalStatement::SeekLastBlock()
{
	auto block = this;

	while (block) {
		if (!block->m_pNext)
			break;

		block = block->m_pNext.get();
	}

	return block;
}

IValue* CRuntimeConditionalStatement::Execute([[maybe_unused]] CRuntimeContext* const ctx)
{

	if (!m_pCondition) { // else
		return ExecuteBlock(ctx);
	}


	auto condition = m_pCondition->Evaluate(ctx);

	if (!condition->IsBooleanConvertible())
		throw CRuntimeError(ctx->m_pRuntime, "the operand is not convertible to a boolean");

	const auto boolValue = condition->ToBoolean();

	if (!condition->HasOwner())
		condition->Release();

	if (boolValue) {
		return ExecuteBlock(ctx);
	}
	
	if (m_pNext) //else (if)
		return m_pNext->Execute(ctx);

	return nullptr;
}