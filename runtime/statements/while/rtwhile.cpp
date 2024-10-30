#include "rtwhile.hpp"
#include "runtime/expressions/rtexpression.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include <ranges>
#include <algorithm>

#include <chrono>


bool CRuntimeWhileStatement::Execute([[maybe_unused]] CFunction* const thisFunction)
{

	while (true) {
		auto condition = m_pCondition->Evaluate(thisFunction);

		if (!condition->ToBoolean()) {
			
			if (!condition->HasOwner())
				condition->Release();

			break;
		}

		if (!condition->HasOwner())
			condition->Release();

		if (ExecuteBlock(thisFunction))
			return true;
	}

	return false;
}