#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"
#include "linter/expressions/ast/ast_main.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "runtime/functions/rtfunction.hpp"

CRuntimeRangedForStatement::CRuntimeRangedForStatement(std::shared_ptr<VariableASTNode>&& iterator, ASTNode&& iterable, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pIterator(std::move(iterator)),
	m_pIterable(std::make_unique<CRuntimeExpression>(std::move(iterable))) {}


CRuntimeRangedForStatement::~CRuntimeRangedForStatement() = default;

IValue* CRuntimeRangedForStatement::Execute(CRuntimeContext* const ctx)
{
	assert(m_pIterable && m_pIterable->HasAST());
	assert(m_pIterator);



	CVariable* iterator{};

	if (m_pIterator->m_bGlobalVariable) {
		auto activeModule = m_pIterator->m_bBelongsToDifferentModule
			? ctx->m_pRuntime->GetModuleByIndex(m_pIterator->m_uModuleIndex)
			: ctx->m_pModule;

		assert(activeModule);
		iterator = activeModule->GetGlobalVariableByIndex(m_pIterator->m_uIndex);
	} else {
		assert(ctx->m_pFunction);
		iterator = ctx->m_pFunction->GetVariableByRef(*m_pIterator);
	}

	assert(iterator);

	auto iteratableExpression = m_pIterable->Evaluate(ctx);

	if (!iteratableExpression->IsIterable()) {
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("\"{}\" is not iterable"), iteratableExpression->TypeAsString()));
	}

	//returns a copy, remember to free!
	auto iter = iteratableExpression->ToIterable();

	for (auto& member : iter) {

		iterator->SetValue(member->Copy());

		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);

			if (lc == lc_break)
				break;
			if (lc == lc_continue)
				continue;

			for (auto& var : iter)
				var->Release();

			if (!iteratableExpression->HasOwner())
				iteratableExpression->Release();

			return v;
		}

	}

	for (auto& var : iter)
		var->Release();

	if (!iteratableExpression->HasOwner())
		iteratableExpression->Release();



	return nullptr;
}

