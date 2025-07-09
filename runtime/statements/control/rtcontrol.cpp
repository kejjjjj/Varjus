#include "varjus_api/internal/structure.hpp"

IValue* CRuntimeLoopControlStatement::Execute([[maybe_unused]]Varjus::CRuntimeContext* const ctx) {
	return reinterpret_cast<IValue*>(static_cast<std::size_t>(m_eCtrl));
}

