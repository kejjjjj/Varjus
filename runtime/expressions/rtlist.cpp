#include "varjus_api/internal/structure.hpp"
#include "varjus_api/types/types.hpp"

VectorOf<IValue*> CRuntimeExpression::EvaluateList(Varjus::CRuntimeContext* const ctx, __ExpressionList& list)
{

	VectorOf<IValue*> args;

	for (auto& l : list) {

		auto value = Evaluate(ctx, l);

		if (value->HasOwner()) {
			args.emplace_back(value->Copy());
		}else {
			args.push_back(value);
		}
	}

	return args;

}
