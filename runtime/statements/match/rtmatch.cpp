#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/types/operators/default_operators.hpp"
#include <algorithm>
#include <ranges>

CRuntimeCaseStatement::CRuntimeCaseStatement(ASTNode&& condition, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {
}
CRuntimeCaseStatement::~CRuntimeCaseStatement() = default;

IValue* CRuntimeCaseStatement::Execute(CRuntimeContext* const ctx) {
	return ExecuteBlock(ctx);
}

CRuntimeMatchStatement::CRuntimeMatchStatement(ASTNode&& condition, CRuntimeCaseStatement* const defaultClause, InstructionSequence&& insns)
	: IRuntimeStructureSequence(std::move(insns)), m_pDefaultCase(defaultClause), 
	m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}

CRuntimeMatchStatement::~CRuntimeMatchStatement() = default;

IValue* CRuntimeMatchStatement::Execute(CRuntimeContext* const ctx) {


	const auto begin = GetCaseIndex(ctx);

	if (begin >= m_oInstructions.size()) {
		if (m_pDefaultCase)
			return m_pDefaultCase->Execute(ctx);

		return nullptr;
	}

	if (const auto v = m_oInstructions[begin]->Execute(ctx)) {
		return v;
	}

	//something
	return nullptr;
}

std::size_t CRuntimeMatchStatement::GetCaseIndex(CRuntimeContext* const ctx) const noexcept
{
	assert(m_pCondition);
	const auto expression = m_pCondition->Evaluate(ctx);
	auto startIndex = std::size_t{ 0 };

	for (const auto & v : m_oInstructions) {

		auto found = false;
		const auto _statement = v->ToCaseStatement();
		assert(_statement);

		if (_statement->IsDefaultClause()) {
			startIndex++;
			continue;
		}

		const auto rhs = _statement->m_pCondition->Evaluate(ctx);
		assert(expression && rhs);

		auto result = OP_STRICT_EQUALITY(ctx->m_pRuntime, expression, rhs);

		if (result->IsBooleanConvertible() && result->ToBoolean()) {
			found = true;
		}

		if (!result->HasOwner())
			result->Release();
		if (!rhs->HasOwner())
			rhs->Release();

		if (found)
			break;

		startIndex++;
	}

	if (!expression->HasOwner())
		expression->Release();

	return startIndex;
}
