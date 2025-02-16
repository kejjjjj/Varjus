#include "coercion.hpp"
#include "default_operators.hpp"

#include "api/types/types.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"

IValue* OP_ASSIGNMENT(IValue* lhs, IValue* rhs)
{
	auto variable = lhs->GetOwner();

	if (lhs->IsImmutable())
		throw CRuntimeError("cannot assign to an immutable value");

	if (!variable)
		throw CRuntimeError("cannot assign to a temporary value");

	//assert(lhs != rhs);
	if (lhs == rhs)
		return lhs;

	switch (rhs->Type()) {
		case t_undefined:
			variable->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
			break;
		case t_boolean:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CBooleanValue>(rhs->ToBoolean()));
			break;
		case t_int:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CIntValue>(rhs->ToInt()));
			break;
		case t_uint:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CUIntValue>(rhs->ToUInt()));
			break;
		case t_double:
			variable->SetValue(CProgramRuntime::AcquireNewValue<CDoubleValue>(rhs->ToDouble()));
			break;
		case t_string:
			variable->SetValue(CStringValue::Construct(rhs->ToString()));
			break;
		case t_callable:
			assert(rhs->ToCallable());
			variable->SetValue(rhs->ToCallable()->Copy());
			break;
		case t_array:
			assert(rhs->ToArray());
			variable->SetValue(rhs->ToArray()->Copy());
			break;
		case t_object:
			assert(rhs->ToObject());
			variable->SetValue(rhs->ToObject()->Copy());
			break;
	}

	lhs->SetOwner(variable);
	assert(lhs->HasOwner());
	return variable->GetValue();
}

IValue* OP_ADDITION(IValue* _lhs, IValue* _rhs)
{
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(static_cast<bool>(lhs->ToBoolean() + rhs->ToBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() + rhs->ToInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->ToUInt() + rhs->ToUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(lhs->ToDouble() + rhs->ToDouble());
		break;
	case t_string:
		result = CStringValue::Construct(lhs->ToString() + rhs->ToString());
		break;
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_SUBTRACTION(IValue* _lhs, IValue* _rhs)
{
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(static_cast<bool>(lhs->ToBoolean() - rhs->ToBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() - rhs->ToInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->ToUInt() - rhs->ToUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(lhs->ToDouble() - rhs->ToDouble());
		break;
	case t_undefined:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_MULTIPLICATION(IValue* _lhs, IValue* _rhs)
{
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(static_cast<bool>(lhs->ToBoolean() * rhs->ToBoolean()));
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() * rhs->ToInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->ToUInt() * rhs->ToUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(lhs->ToDouble() * rhs->ToDouble());
		break;
	case t_undefined:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_DIVISION(IValue* _lhs, IValue* _rhs)
{
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };
	
	switch (lhs->Type()) {
	case t_int:

		if(rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() / rhs->ToInt());
		break;

	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->ToUInt() / rhs->ToUInt());
		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(lhs->ToDouble() / rhs->ToDouble());
		break;

	case t_undefined:
	case t_boolean:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_MODULO(IValue* _lhs, IValue* _rhs)
{
	auto&& [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:

		if (rhs->ToInt() == 0ll)
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->ToInt() % rhs->ToInt());
		break;
	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->ToUInt() & rhs->ToUInt());
		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError("division by 0");

		result = CProgramRuntime::AcquireNewValue<CDoubleValue>(std::fmod(lhs->ToDouble(), rhs->ToDouble()));
		break;

	case t_undefined:
	case t_boolean:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_LESS_THAN(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() < rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() < rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() < rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() < rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_LESS_EQUAL(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() <= rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() <= rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() <= rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() <= rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_GREATER_THAN(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() > rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() > rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() > rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() > rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_GREATER_EQUAL(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() >= rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() >= rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() >= rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() >= rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_EQUALITY(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(true); // undefined == undefined is always true
		break;
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() == rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() == rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() == rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() == rhs->AsDouble());
		break;
	case t_string:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->ToString() == rhs->ToString());
		break;

	case t_callable:
	case t_array:
	case t_object:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AddressOf() == rhs->AddressOf());
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_UNEQUALITY(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(false); // undefined != undefined is always false
		break;
	case t_boolean:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsBoolean() != rhs->AsBoolean());
		break;
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsInt() != rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsUInt() != rhs->AsUInt());
		break;
	case t_double:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AsDouble() != rhs->AsDouble());
		break;
	case t_string:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->ToString() != rhs->ToString());
		break;
	case t_callable:
	case t_array:
	case t_object:
		result = CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->AddressOf() != rhs->AddressOf());
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}

IValue* OP_STRICT_EQUALITY(IValue* lhs, IValue* rhs)
{
	if(lhs->Type() != rhs->Type())
		return CProgramRuntime::AcquireNewValue<CBooleanValue>(false);

	return OP_EQUALITY(lhs, rhs);
}
IValue* OP_STRICT_UNEQUALITY(IValue* lhs, IValue* rhs)
{
	if (lhs->Type() != rhs->Type())
		return CProgramRuntime::AcquireNewValue<CBooleanValue>(true);

	return OP_UNEQUALITY(lhs, rhs);
}

IValue* OP_LOGICAL_AND(IValue* lhs, IValue* rhs)
{
	if(!lhs->IsBooleanConvertible())
		throw CRuntimeError(std::format("\"{}\" is not convertible to a boolean", lhs->TypeAsString()));

	if (!rhs->IsBooleanConvertible())
		throw CRuntimeError(std::format("\"{}\" is not convertible to a boolean", rhs->TypeAsString()));

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->ToBoolean() && rhs->ToBoolean());
}
IValue* OP_LOGICAL_OR(IValue* lhs, IValue* rhs)
{
	if (!lhs->IsBooleanConvertible())
		throw CRuntimeError(std::format("\"{}\" is not convertible to a boolean", lhs->TypeAsString()));

	if (!rhs->IsBooleanConvertible())
		throw CRuntimeError(std::format("\"{}\" is not convertible to a boolean", rhs->TypeAsString()));

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(lhs->ToBoolean() || rhs->ToBoolean());
}
IValue* OP_LEFT_SHIFT(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->AsInt() << rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->AsUInt() << rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only shift int operands, had \"{}\" and \"{}\"", 
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_RIGHT_SHIFT(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->AsInt() >> rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->AsUInt() >> rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_BITWISE_OR(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->AsInt() | rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->AsUInt() | rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_BITWISE_XOR(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->AsInt() ^ rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->AsUInt() ^ rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
IValue* OP_BITWISE_AND(IValue* _lhs, IValue* _rhs)
{
	auto [lhs, rhs, alloc] = Coerce(_lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CProgramRuntime::AcquireNewValue<CIntValue>(lhs->AsInt() & rhs->AsInt());
		break;
	case t_uint:
		result = CProgramRuntime::AcquireNewValue<CUIntValue>(lhs->AsUInt() & rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}

inline static void OP_ASSIGNMENT_CHECK(IValue* lhs, IValue* rhs)
{
	if (lhs->IsImmutable())
		throw CRuntimeError("cannot assign to an immutable value");

	if (!lhs->GetOwner())
		throw CRuntimeError("cannot assign to a temporary value");

	if (lhs->Type() != rhs->Type())
		throw CRuntimeError(std::format("can't assign \"{}\" to \"{}\"", rhs->TypeAsString(), lhs->TypeAsString()));
}

IValue* OP_ASSIGNMENT_ADDITION(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() += rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() += rhs->AsUInt();
		break;
	case t_double:
		lhs->AsDouble() += rhs->AsDouble();
		break;
	case t_string:
		lhs->AsString() += rhs->AsString();
		break;
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_SUBTRACTION(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() -= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() -= rhs->AsUInt();
		break;
	case t_double:
		lhs->AsDouble() -= rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_MULTIPLICATION(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() *= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() *= rhs->AsUInt();
		break;
	case t_double:
		lhs->AsDouble() *= rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_DIVISION(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:

		if (rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError("division by 0");

		lhs->AsInt() /= rhs->AsInt();

		break;
	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError("division by 0");

		lhs->AsUInt() /= rhs->AsUInt();

		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError("division by 0");

		lhs->AsDouble() /= rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_MODULO(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:

		if (rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError("division by 0");

		lhs->AsInt() %= rhs->AsInt();

		break;
	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError("division by 0");

		lhs->AsUInt() %= rhs->AsUInt();

		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError("division by 0");

		lhs->AsDouble() = std::fmod(lhs->AsDouble(), rhs->AsDouble());
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_LEFT_SHIFT(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() <<= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() <<= rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
IValue* OP_ASSIGNMENT_RIGHT_SHIFT(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() >>= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() >>= rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;

}
IValue* OP_ASSIGNMENT_BITWISE_OR(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() |= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() |= rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;

}
IValue* OP_ASSIGNMENT_BITWISE_XOR(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() ^= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() ^= rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;

}
IValue* OP_ASSIGNMENT_BITWISE_AND(IValue* lhs, IValue* rhs)
{
	OP_ASSIGNMENT_CHECK(lhs, rhs);

	switch (lhs->Type()) {

	case t_int:
		lhs->AsInt() &= rhs->AsInt();
		break;
	case t_uint:
		lhs->AsUInt() &= rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	return lhs;
}
