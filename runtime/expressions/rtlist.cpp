#include "runtime/structure.hpp"
#include "runtime/values/types/types.hpp"
VectorOf<IValue*> CRuntimeExpression::EvaluateList(CRuntimeContext* const ctx, const ExpressionList& list)
{

	VectorOf<IValue*> args;

	for (auto& l : list) {

		auto value = Evaluate(ctx, l.get());

		if (value->HasOwner()) {
			args.emplace_back(value->Copy());
		}else {
			args.push_back(value);
		}
	}

	return args;

}
