#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

using namespace Varjus;

CRuntimeInitialization::CRuntimeInitialization(std::unique_ptr<CInitializer>&& init) : m_oData(std::move(init)) {}
CRuntimeInitialization::~CRuntimeInitialization() = default;

void CRuntimeInitialization::EvaluateArray(Varjus::CRuntimeContext* const ctx, CInitializer& init, IValue* const getter)
{

	assert(init.m_pInitializer);
	const auto arr = getter->ToArray();

	if(!arr)
		throw CRuntimeError(ctx->m_pRuntime, VSL("array destructuring requires an array as the initializer"));

	auto& target = std::get<1>(init.m_pTarget);
	assert(target);

	auto& data = std::get<0>(target->m_oData);

	const auto& internal = arr->Internal();
	const auto restProperty = data.m_pRest.get();

	if(data.m_oProperties.size() > internal->Length())
		throw CRuntimeError(ctx->m_pRuntime, VSL("the array size is smaller than the count of destructured elements"));

	auto& vars = internal->GetContent().GetVariables();

	for (std::size_t i{}; const auto& prop : data.m_oProperties) {
		
		auto& val = vars[i++];
		auto receiver = CRuntimeExpression::EvaluateVariable(ctx, prop.get());
		assert(val);
		assert(receiver && receiver->HasOwner());
		[[maybe_unused]] const auto result = OP_ASSIGNMENT(ctx->m_pRuntime, receiver, val->GetValue());
	}

	if (restProperty) {
		auto receiver = CRuntimeExpression::EvaluateVariable(ctx, restProperty);
		assert(receiver && receiver->HasOwner());
		const auto ignoredIndices = data.m_oProperties.size();

		IValues vals;
		if(!vars.empty())
			vals.reserve(vars.size() - std::size_t(1));

		for (const auto& val : internal->GetContent().GetVariables() | std::views::drop(ignoredIndices)) {
			vals.push_back(val->GetValue()->Copy());
		}

		receiver->GetOwner()->SetValue(CArrayValue::Construct(ctx->m_pRuntime, std::move(vals)));

	}

	if (!getter->HasOwner())
		getter->Release();

}
void CRuntimeInitialization::EvaluateObject(Varjus::CRuntimeContext* const ctx, CInitializer& init, IValue* const getter)
{
	assert(init.m_pInitializer);
	const auto obj = getter->ToObject();

	if (!obj)
		throw CRuntimeError(ctx->m_pRuntime, VSL("object destructuring requires an object as the initializer"));

	auto& target = std::get<1>(init.m_pTarget);
	assert(target);

	auto& data = std::get<1>(target->m_oData);

	for (const auto& [variable, memberIdx] : data) {
		assert(variable.get());
		auto receiver = CRuntimeExpression::EvaluateVariable(ctx, variable.get());
		assert(receiver && receiver->HasOwner());

		auto property = obj->GetAggregate(ctx, memberIdx);
		receiver->GetOwner()->SetValue(property->Copy());
	}

	if (!getter->HasOwner())
		getter->Release();

}
void CRuntimeInitialization::EvaluateInitializer(Varjus::CRuntimeContext* const ctx, CInitializer& data, IValue* const getter)
{
	//no actual declaration - just an expression
	if (!data.m_bContainsDeclaration) {

		if (!getter) //for example a for loop without an expression
			return;

		if (!getter->HasOwner())
			getter->Release();

		return;
	}

	if (data.m_eTargetType == EInitializationTarget::singular) {
		auto var = CRuntimeExpression::EvaluateVariable(ctx, std::get<0>(data.m_pTarget)->GetVariable());
		assert(var && var->HasOwner());

		if (!getter)
			return;

		//no need to release result as it's just the lhs (a variable)
		[[maybe_unused]] const auto result = OP_ASSIGNMENT(ctx->m_pRuntime, var, getter);

		if (!getter->HasOwner())
			getter->Release();

		return;
	}

	auto& target = std::get<1>(data.m_pTarget);
	assert(target);

	if (target->m_eType == EDestructuredType::dest_array) {
		EvaluateArray(ctx, data, getter);
		return;
	}

	EvaluateObject(ctx, data, getter);
	return;
}
IValue* CRuntimeInitialization::Execute(Varjus::CRuntimeContext* const ctx)
{
	assert(m_oData);

	if (!m_oData->m_pInitializer)
		return nullptr;

	EvaluateInitializer(ctx, *m_oData, CRuntimeExpression::Evaluate(ctx, m_oData->m_pInitializer));
	return nullptr;
}