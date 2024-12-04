#include "rtconditional.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"
#include <runtime/exceptions/exception.hpp>


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

IValue* CRuntimeConditionalStatement::Execute([[maybe_unused]] CFunction* const thisFunction)
{

	if (!m_pCondition) { // else
		return ExecuteBlock(thisFunction);
	}


	auto condition = m_pCondition->Evaluate(thisFunction);

	if (!condition->IsBooleanConvertible())
		throw CRuntimeError("the operand is not convertible to a boolean");

	const auto boolValue = condition->ToBoolean();

	if (!condition->HasOwner())
		condition->Release();

	if (boolValue) {
		return ExecuteBlock(thisFunction);
	}
	
	if (m_pNext) //else (if)
		return m_pNext->Execute(thisFunction);

	return nullptr;
}