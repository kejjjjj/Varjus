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
				assert(CProgramRuntime::GetExceptionValue());
				return ExecuteCatchBlock(ctx);
			}

			return rv;
		}

	}

	return nullptr;
}
IValue* CRuntimeTryCatchStatement::ExecuteCatchBlock(CRuntimeContext* const ctx)
{
	CProgramRuntime::CatchException();

	auto catchVar = ctx->m_pFunction->GetVariableByRef(m_uCatchVariable);
	assert(catchVar && catchVar->GetValue());

	auto& value = catchVar->GetValue();
	value->MakeImmutable();

	auto& ex = CProgramRuntime::GetExceptionValue();

	//set new value
	catchVar->SetValue(ex->Copy());
	ex->Release();
	ex = nullptr;

	for (auto& insn : m_oCatchInstructions) {
		if (auto rv = insn->Execute(ctx)) {
			return rv;
		}
	}

	return nullptr;
}

