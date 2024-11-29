#include "runtime/structure.hpp"


IValue* CRuntimeLoopControlStatement::Execute([[maybe_unused]]CFunction* const thisFunction) {
	return reinterpret_cast<IValue*>(static_cast<std::size_t>(m_eCtrl));
}

