#include "runtime/structure.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
CRuntimeTryCatchStatement::CRuntimeTryCatchStatement(VariableIndex catchVariable, 
	InstructionSequence&& tryBlock, InstructionSequence&& catchBlock)
	: m_uCatchVariableIndex(catchVariable),
	m_oTryInstructions(std::move(tryBlock)), m_oCatchInstructions(std::move(catchBlock)){}

CRuntimeTryCatchStatement::~CRuntimeTryCatchStatement() = default;

IValue* CRuntimeTryCatchStatement::Execute(CFunction* const thisFunction)
{

	for (auto& insn : m_oTryInstructions) {
		if (auto rv = insn->Execute(thisFunction)) {
			if (CProgramRuntime::ExceptionThrown()) {
				return ExecuteCatchBlock(thisFunction, rv);
			}

			return rv;
		}

	}

	return nullptr;
}
IValue* CRuntimeTryCatchStatement::ExecuteCatchBlock(CFunction* const thisFunction, IValue* ex)
{
	CProgramRuntime::CatchException();

	auto catchVar = thisFunction->GetVariableByIndex(m_uCatchVariableIndex);
	assert(catchVar && catchVar->GetValue());

	auto& value = catchVar->GetValue();
	value->MakeImmutable();

	//set new value
	catchVar->SetValue(ex->HasOwner() ? ex->Copy() : ex);

	for (auto& insn : m_oCatchInstructions) {
		if (auto rv = insn->Execute(thisFunction)) {
			return rv;
		}
	}

	return nullptr;
}

