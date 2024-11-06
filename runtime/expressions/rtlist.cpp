#include "runtime/structure.hpp"
#include "runtime/values/types/types.hpp"
VectorOf<IValue*> CRuntimeExpression::EvaluateList(CFunction* const thisFunction, const ExpressionList& list)
{

	VectorOf<IValue*> args;

	for (auto& l : list) {

		auto value = Evaluate(thisFunction, l.get());

		if (value->HasOwner()) {
			args.emplace_back(value->Copy());
			//value->Release();
		}else {
			args.emplace_back(value);
		}
	}
	return args;

}
