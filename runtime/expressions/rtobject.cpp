#include "varjus_api/internal/structure.hpp"
#include "varjus_api/types/default.hpp"

ObjectInitializer CRuntimeExpression::EvaluateObject(CRuntimeContext* const ctx, ObjectInitializerData& obj)
{
	ObjectInitializer init;
	for (auto& [key, value] : obj) {
		auto v = Evaluate(ctx, value);
		init.emplace_back(std::make_pair(key, v->HasOwner() ? v->Copy() : v));
	}

	return init;
}

