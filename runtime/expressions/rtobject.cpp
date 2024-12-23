#include "runtime/structure.hpp"
#include "runtime/values/types/simple.hpp"

ObjectInitializer CRuntimeExpression::EvaluateObject(CRuntimeContext* const ctx, const ObjectInitializerData& obj)
{
	ObjectInitializer init;
	for (auto& [key, value] : obj) {
		auto v = Evaluate(ctx, value.get());
		init.emplace_back(std::make_pair(key, v->HasOwner() ? v->Copy() : v));
	}

	return init;
}

