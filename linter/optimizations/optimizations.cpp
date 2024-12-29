#include "optimizations.hpp"

#define VALUEPOOL_INIT_SIZE size_t(100)

template<> COwningObjectPool<IConstEvalValue>     COptimizationValues::m_oValuePool<IConstEvalValue>   (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConstEvalIntValue>  COptimizationValues::m_oValuePool<CConstEvalIntValue>(VALUEPOOL_INIT_SIZE);
