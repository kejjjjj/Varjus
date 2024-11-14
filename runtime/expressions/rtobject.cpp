#include "runtime/structure.hpp"

ObjectInitializer CRuntimeExpression::EvaluateObject(CFunction* const thisFunction, const ObjectInitializerData& obj)
{
	ObjectInitializer init;
	for (auto& [key, value] : obj) {

		init.emplace_back(
			std::make_pair(
				key, 
				Evaluate(thisFunction, value.get())
			)
		);

	}

	return init;
}

