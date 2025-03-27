#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"

CRuntimeForStatement::CRuntimeForStatement(
	ASTNode&& init,
	ASTNode&& cond,
	ASTNode&& endExpr, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pInitializer(std::make_unique<CRuntimeExpression>(std::move(init))),
	m_pCondition(std::make_unique<CRuntimeExpression>(std::move(cond))),
	m_pOnEnd(std::make_unique<CRuntimeExpression>(std::move(endExpr)))
{
}


CRuntimeForStatement::~CRuntimeForStatement() = default;

IValue* CRuntimeForStatement::Execute(CRuntimeContext* const ctx)
{

	if (m_pInitializer->HasAST()) {
		auto v = m_pInitializer->Evaluate(ctx);
		if (!v->HasOwner())
			v->Release();
	}

	bool firstIter = true;

	while (true) {

		if (!firstIter && m_pOnEnd->HasAST()) {
			auto v = m_pOnEnd->Evaluate(ctx);
			if (!v->HasOwner())
				v->Release();
		}

		firstIter = false;

		if (m_pCondition->HasAST()) {
			auto condition = m_pCondition->Evaluate(ctx);

			if (!condition->IsBooleanConvertible())
				throw CRuntimeError(ctx->m_pRuntime, VSL("the operand is not convertible to a boolean"));

			const auto boolValue = condition->ToBoolean();

			if (!condition->HasOwner())
				condition->Release();

			if (!boolValue) {
				break;
			}
		}

		if (auto v = ExecuteBlock(ctx)) {
			const auto lc = IRuntimeStructure::ToControlStatement(v);

			if (lc == lc_break)
				break;
			if (lc == lc_continue)
				continue;

			return v;
		}

	}

	return nullptr;
}

