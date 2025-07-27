#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "linter/expressions/ast/ast_main.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/structure.hpp"

using namespace Varjus;

CRuntimeRangedForStatement::CRuntimeRangedForStatement(std::unique_ptr<CInitializer>&& iterator, __InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pIterator(std::move(iterator)) {}


CRuntimeRangedForStatement::~CRuntimeRangedForStatement() = default;

IValue* CRuntimeRangedForStatement::Execute(Varjus::CRuntimeContext* const ctx)
{
	assert(m_pIterator && m_pIterator->m_pInitializer);

	if (m_pIterator->m_eTargetType == EInitializationTarget::singular)
		return ExecuteSingular(ctx, m_pIterator->GetVariable());

	return ExecuteDestructured(ctx);
}

IValue* CRuntimeRangedForStatement::GetIterable(Varjus::CRuntimeContext* const ctx)
{
	assert(m_pIterator && m_pIterator->m_pInitializer);

	auto iterableExpression = CRuntimeExpression::Evaluate(ctx, m_pIterator->m_pInitializer);
	assert(iterableExpression);

	if (!iterableExpression->IsIterable()) {
		throw CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("\"{}\" is not iterable"), iterableExpression->TypeAsString()));
	}

	return iterableExpression;
}

IValue* CRuntimeRangedForStatement::ExecuteSingular(Varjus::CRuntimeContext* const ctx, const DestructuredVariableRef& var)
{
	CVariable* iterator{};

	if (var->m_bGlobalVariable) {
		auto activeModule = var->m_bBelongsToDifferentModule
			? ctx->m_pRuntime->GetModuleByIndex(var->m_uModuleIndex)
			: ctx->m_pModule;

		assert(activeModule);
		iterator = activeModule->GetGlobalVariableByIndex(var->m_uIndex);
	}
	else {
		assert(ctx->m_pFunction);
		iterator = ctx->m_pFunction->GetVariableByRef(*var);
	}

	assert(iterator);

	auto iterableExpression = GetIterable(ctx);
	//returns a copy, remember to free!
	auto iter = iterableExpression->ToIterable();

	const auto ReleaseStuff = [&iterableExpression](IValues& arr) {
		for (auto& var : arr)
			var->Release();

		if (!iterableExpression->HasOwner())
			iterableExpression->Release();
	};

	for (auto& member : iter) {
		iterator->SetValue(member->Copy());

		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);

			if (lc == lc_break)
				break;
			if (lc == lc_continue)
				continue;

			ReleaseStuff(iter);
			return v;
		}

	}

	ReleaseStuff(iter);
	return nullptr;
}
IValue* CRuntimeRangedForStatement::ExecuteDestructured(Varjus::CRuntimeContext* const ctx)
{

	auto iterableExpression = GetIterable(ctx);
	auto iter = iterableExpression->ToIterable();
	auto& initializer = *m_pIterator;

	const auto ReleaseStuff = [&iterableExpression](IValues& arr) {
		for (auto& var : arr)
			var->Release();

		if (!iterableExpression->HasOwner())
			iterableExpression->Release();
	};

	for (auto& member : iter) {
		CRuntimeInitialization::EvaluateInitializer(ctx, initializer, member->Copy());

		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);

			if (lc == lc_break)
				break;
			if (lc == lc_continue)
				continue;

			ReleaseStuff(iter);
			return v;
		}

	}

	ReleaseStuff(iter);
	return nullptr;
}
