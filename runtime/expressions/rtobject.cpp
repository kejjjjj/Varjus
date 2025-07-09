#include "varjus_api/internal/structure.hpp"
#include "varjus_api/types/default.hpp"

__ObjectInitializer CRuntimeExpression::EvaluateObject(Varjus::CRuntimeContext* const ctx, __ObjectInitializerData& obj)
{
	__ObjectInitializer init;
	for (auto& [key, value] : obj) {
		auto v = Evaluate(ctx, value);
		init.emplace_back(std::make_pair(key, v->HasOwner() ? v->Copy() : v));
	}

	return init;
}

