#include "runtime/structure.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/variables.hpp"
CRuntimeTryCatchStatement::CRuntimeTryCatchStatement(const CCrossModuleReference& catchVariable,
	InstructionSequence&& tryBlock, InstructionSequence&& catchBlock)
	: m_uCatchVariable(catchVariable),
	m_oTryInstructions(std::move(tryBlock)), m_oCatchInstructions(std::move(catchBlock)){}

CRuntimeTryCatchStatement::~CRuntimeTryCatchStatement() = default;

IValue* CRuntimeTryCatchStatement::Execute(CRuntimeContext* const ctx)
{

	for (auto& insn : m_oTryInstructions) {
		if (auto rv = insn->Execute(ctx)) {
			if (CProgramRuntime::ExceptionThrown()) {
				return ExecuteCatchBlock(ctx, rv);
			}

			return rv;
		}

	}

	return nullptr;
}
IValue* CRuntimeTryCatchStatement::ExecuteCatchBlock(CRuntimeContext* const ctx, IValue* ex)
{
	CProgramRuntime::CatchException();

	auto catchVar = ctx->m_pFunction->GetVariableByRef(m_uCatchVariable);
	assert(catchVar && catchVar->GetValue());

	auto& value = catchVar->GetValue();
	value->MakeImmutable();

	//set new value
	catchVar->SetValue(ex->HasOwner() ? ex->Copy() : ex);

	for (auto& insn : m_oCatchInstructions) {
		if (auto rv = insn->Execute(ctx)) {
			return rv;
		}
	}

	return nullptr;
}

