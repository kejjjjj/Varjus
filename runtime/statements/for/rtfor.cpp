#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"
#include "linter/expressions/ast.hpp"

using namespace Varjus;

CRuntimeForStatement::CRuntimeForStatement(
	std::unique_ptr<CInitializer>&& init,
	ASTNode&& cond,
	ASTNode&& endExpr, __InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), 
	m_pInit(std::move(init)),
	m_pCondition(std::make_unique<CRuntimeExpression>(std::move(cond))),
	m_pOnEnd(std::make_unique<CRuntimeExpression>(std::move(endExpr)))
{
}


CRuntimeForStatement::~CRuntimeForStatement() = default;

IValue* CRuntimeForStatement::Execute(Varjus::CRuntimeContext* const ctx)
{

	if (m_pInit) {
		CRuntimeInitialization::EvaluateInitializer(ctx, *m_pInit, CRuntimeExpression::Evaluate(ctx, m_pInit->m_pInitializer));
	}

	bool firstIter = true;

	while (true) {

		if (!firstIter && m_pOnEnd->HasAST()) {
			auto v = m_pOnEnd->EvaluateExpression(ctx);
			if (!v->HasOwner())
				v->Release();
		}

		firstIter = false;

		if (m_pCondition->HasAST()) {
			auto condition = m_pCondition->EvaluateExpression(ctx);

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

