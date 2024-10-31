#include "rtconditional.hpp"
#include "runtime/expressions/rtexpression.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"


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

bool CRuntimeConditionalStatement::Execute([[maybe_unused]] CFunction* const thisFunction)
{

	if (!m_pCondition) {
		return ExecuteBlock(thisFunction);
	}


	auto condition = m_pCondition->Evaluate(thisFunction);
	const auto boolValue = condition->ToBoolean();

	if (!condition->HasOwner())
		condition->Release();

	if (boolValue) {
		return ExecuteBlock(thisFunction);
	}
	
	if (m_pNext)
		return m_pNext->Execute(thisFunction);

	return false;
}