#include "api/types/types.hpp"

#include "linter/context.hpp"

#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/structure.hpp"


#define START_METHOD(name) \
if(!_this)\
	throw CRuntimeError("attempted to call a method without \"this\" context"); \
if(_this->Type() != t_object) \
	throw CRuntimeError(std::format("object.{} expected an object, but got {}", #name, _this->TypeAsString()));\
auto name = _this->ToObject()

BuiltInMethod_t CObjectValue::m_oMethods;
BuiltInProperty_t CObjectValue::m_oProperties;

FORWARD_DECLARE_METHOD(Object_Keys);
FORWARD_DECLARE_METHOD(Object_Values);

void CObjectValue::ConstructMethods()
{
	m_oMethods.clear();

	m_oMethods.AddMethod("keys", Object_Keys, 0u);
	m_oMethods.AddMethod("values", Object_Values, 0u);

}


FORWARD_DECLARE_PROPERTY(ObjectLength);
void CObjectValue::ConstructProperties()
{
	m_oProperties.clear();
	m_oProperties.AddProperty("length", ObjectLength);
}

DEFINE_PROPERTY(ObjectLength) {
	START_METHOD(__this);
	return CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<std::int64_t>(__this->Internal()->GetAggregateValue().Length()));
}


DEFINE_METHOD(Object_Keys)
{
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetAggregateValue().Iterator();

	IValues results(vars.size());

	//result array
	for (auto i = std::size_t(0); const auto& [index, var] : vars) {
		results[i++] = CStringValue::Construct(CFileContext::m_oAllMembers.At(index));
	}

	return CArrayValue::Construct(std::move(results));
}
DEFINE_METHOD(Object_Values)
{
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetAggregateValue().Iterator();

	IValues results(vars.size());

	//result array
	for (auto i = std::size_t(0); const auto & [index, var] : vars) {
		results[i++] = var->GetValue()->Copy();
	}

	return CArrayValue::Construct(std::move(results));
}