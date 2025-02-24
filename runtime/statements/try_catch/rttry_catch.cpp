#include "api/types/types.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/structure.hpp"
#include "api/internal/variables.hpp"

CRuntimeTryCatchStatement::CRuntimeTryCatchStatement(const CCrossModuleReference& catchVariable,
	InstructionSequence&& tryBlock, InstructionSequence&& catchBlock)
	: m_uCatchVariable(catchVariable),
	m_oTryInstructions(std::move(tryBlock)), m_oCatchInstructions(std::move(catchBlock)){}

CRuntimeTryCatchStatement::~CRuntimeTryCatchStatement() = default;

IValue* CRuntimeTryCatchStatement::Execute(CRuntimeContext* const ctx)
{

	for (auto& insn : m_oTryInstructions) {
		if (auto rv = insn->Execute(ctx)) {
			if (ctx->m_pRuntime->ExceptionThrown()) {
				assert(ctx->m_pRuntime->GetExceptionValue());
				return ExecuteCatchBlock(ctx);
			}

			return rv;
		}

	}

	return nullptr;
}
IValue* CRuntimeTryCatchStatement::ExecuteCatchBlock(CRuntimeContext* const ctx)
{
	ctx->m_pRuntime->CatchException();

	auto catchVar = ctx->m_pFunction->GetVariableByRef(m_uCatchVariable);
	assert(catchVar && catchVar->GetValue());

	auto& value = catchVar->GetValue();
	value->MakeImmutable();

	auto& ex = ctx->m_pRuntime->GetExceptionValue();

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

