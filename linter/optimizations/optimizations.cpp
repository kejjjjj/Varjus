#ifdef OPTIMIZATIONS

#include "optimizations.hpp"

#define VALUEPOOL_INIT_SIZE size_t(100)

template<> COwningObjectPool<CConstEvalVariable>     COptimizationValues::m_oValuePool<CConstEvalVariable>    (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<IConstEvalValue>        COptimizationValues::m_oValuePool<IConstEvalValue>       (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConstEvalBooleanValue> COptimizationValues::m_oValuePool<CConstEvalBooleanValue>(VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConstEvalIntValue>     COptimizationValues::m_oValuePool<CConstEvalIntValue>    (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConstEvalDoubleValue>  COptimizationValues::m_oValuePool<CConstEvalDoubleValue> (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConstEvalStringValue>  COptimizationValues::m_oValuePool<CConstEvalStringValue> (VALUEPOOL_INIT_SIZE);

#endif