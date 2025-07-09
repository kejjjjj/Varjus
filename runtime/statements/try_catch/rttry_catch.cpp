#include "varjus_api/types/types.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"

CRuntimeTryCatchStatement::CRuntimeTryCatchStatement(const CCrossModuleReference& catchVariable,
	__InstructionSequence&& tryBlock, __InstructionSequence&& catchBlock)
	: m_uCatchVariable(catchVariable),
	m_oTryInstructions(std::move(tryBlock)), m_oCatchInstructions(std::move(catchBlock)){}

CRuntimeTryCatchStatement::~CRuntimeTryCatchStatement() = default;

IValue* CRuntimeTryCatchStatement::Execute(Varjus::CRuntimeContext* const ctx)
{

	for (auto& insn : m_oTryInstructions) {
		if (auto rv = insn->Execute(ctx)) {
			if (ctx->m_pRuntime->ExceptionThrown()) {
				return ExecuteCatchBlock(ctx);
			}

			return rv;
		}

	}

	return nullptr;
}
IValue* CRuntimeTryCatchStatement::ExecuteCatchBlock(Varjus::CRuntimeContext* const ctx)
{

	ctx->m_pRuntime->CatchException();

	auto catchVar = ctx->m_pFunction->GetVariableByRef(m_uCatchVariable);
	assert(catchVar && catchVar->GetValue());

	auto& value = catchVar->GetValue();
	value->MakeImmutable();

	auto& ex = ctx->m_pRuntime->GetExceptionValue();
	assert(ex->GetAllocator());
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

