#include "runtime/structure.hpp"
#include "runtime/values/types/simple.hpp"

ObjectInitializer CRuntimeExpression::EvaluateObject(CFunction* const thisFunction, const ObjectInitializerData& obj)
{
	ObjectInitializer init;
	for (auto& [key, value] : obj) {

		auto v = Evaluate(thisFunction, value.get());


		init.emplace_back(
			std::make_pair(
				key,
				v->HasOwner() ? v->Copy() : v
			)
		);

	}

	return init;
}

