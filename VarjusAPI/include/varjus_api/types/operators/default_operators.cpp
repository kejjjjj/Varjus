#include "coercion.hpp"
#include "default_operators.hpp"

#include "varjus_api/types/types.hpp"
#include "runtime/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"

#include <cmath>

#define DEFINE_OPERATOR(Name)\
IValue* Name([[maybe_unused]]CProgramRuntime* const runtime, IValue* _lhs, IValue* _rhs)

DEFINE_OPERATOR(OP_ASSIGNMENT)
{
	auto variable = _lhs->GetOwner();

	if (_lhs->IsImmutable())
		throw CRuntimeError(runtime, "cannot assign to an immutable value");

	if (!variable)
		throw CRuntimeError(runtime, "cannot assign to a temporary value");

	//assert(lhs != rhs);
	if (_lhs == _rhs)
		return _lhs;

	switch (_rhs->Type()) {
		case t_undefined:
			variable->SetValue(IValue::Construct(runtime));
			break;
		case t_boolean:
			variable->SetValue(CBooleanValue::Construct(runtime, _rhs->ToBoolean()));
			break;
		case t_int:
			variable->SetValue(CIntValue::Construct(runtime, _rhs->ToInt()));
			break;
		case t_uint:
			variable->SetValue(CUIntValue::Construct(runtime, _rhs->ToUInt()));
			break;
		case t_double:
			variable->SetValue(CDoubleValue::Construct(runtime, _rhs->ToDouble()));
			break;
		case t_string:
			variable->SetValue(CStringValue::Construct(runtime, _rhs->ToString()));
			break;
		case t_callable:
			assert(_rhs->ToCallable());
			variable->SetValue(_rhs->ToCallable()->Copy());
			break;
		case t_array:
			assert(_rhs->ToArray());
			variable->SetValue(_rhs->ToArray()->Copy());
			break;
		case t_object:
			assert(_rhs->ToObject());
			variable->SetValue(_rhs->ToObject()->Copy());
			break;
	}

	_lhs->SetOwner(variable);
	assert(_lhs->HasOwner());
	return variable->GetValue();
}
DEFINE_OPERATOR(OP_ADDITION)
{
	auto&& [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, static_cast<bool>(lhs->ToBoolean() + rhs->ToBoolean()));
		break;
	case t_int:
		result = CIntValue::Construct(runtime, lhs->ToInt() + rhs->ToInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->ToUInt() + rhs->ToUInt());
		break;
	case t_double:
		result = CDoubleValue::Construct(runtime, lhs->ToDouble() + rhs->ToDouble());
		break;
	case t_string:
		result = CStringValue::Construct(runtime, lhs->ToString() + rhs->ToString());
		break;
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_SUBTRACTION)
{
	auto&& [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, static_cast<bool>(lhs->ToBoolean() - rhs->ToBoolean()));
		break;
	case t_int:
		result = CIntValue::Construct(runtime, lhs->ToInt() - rhs->ToInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->ToUInt() - rhs->ToUInt());
		break;
	case t_double:
		result = CDoubleValue::Construct(runtime, lhs->ToDouble() - rhs->ToDouble());
		break;
	case t_undefined:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_MULTIPLICATION)
{
	auto&& [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->ToBoolean() && rhs->ToBoolean());
		break;
	case t_int:
		result = CIntValue::Construct(runtime, lhs->ToInt() * rhs->ToInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->ToUInt() * rhs->ToUInt());
		break;
	case t_double:
		result = CDoubleValue::Construct(runtime, lhs->ToDouble() * rhs->ToDouble());
		break;
	case t_undefined:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_DIVISION)
{
	auto&& [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };
	
	switch (lhs->Type()) {
	case t_int:

		if(rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError(runtime, "division by 0");

		result = CIntValue::Construct(runtime, lhs->ToInt() / rhs->ToInt());
		break;

	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError(runtime, "division by 0");

		result = CUIntValue::Construct(runtime, lhs->ToUInt() / rhs->ToUInt());
		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError(runtime, "division by 0");

		result = CDoubleValue::Construct(runtime, lhs->ToDouble() / rhs->ToDouble());
		break;

	case t_undefined:
	case t_boolean:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_MODULO)
{
	auto&& [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:

		if (rhs->ToInt() == 0ll)
			throw CRuntimeError(runtime, "division by 0");

		result = CIntValue::Construct(runtime, lhs->ToInt() % rhs->ToInt());
		break;
	case t_uint:

		if (rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError(runtime, "division by 0");

		result = CUIntValue::Construct(runtime, lhs->ToUInt() % rhs->ToUInt());
		break;
	case t_double:

		if (rhs->ToDouble() == 0.0)
			throw CRuntimeError(runtime, "division by 0");

		result = CDoubleValue::Construct(runtime, std::fmod(lhs->ToDouble(), rhs->ToDouble()));
		break;

	case t_undefined:
	case t_boolean:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_LESS_THAN)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() < rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() < rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() < rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() < rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_LESS_EQUAL)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() <= rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() <= rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() <= rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() <= rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_GREATER_THAN)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() > rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() > rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() > rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() > rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_GREATER_EQUAL)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() >= rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() >= rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() >= rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() >= rhs->AsDouble());
		break;
	case t_string:
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_EQUALITY)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		result = CBooleanValue::Construct(runtime, true); // undefined == undefined is always true
		break;
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() == rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() == rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() == rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() == rhs->AsDouble());
		break;
	case t_string:
		result = CBooleanValue::Construct(runtime, lhs->ToString() == rhs->ToString());
		break;

	case t_callable:
	case t_array:
	case t_object:
		result = CBooleanValue::Construct(runtime, lhs->AddressOf() == rhs->AddressOf());
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_UNEQUALITY)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_undefined:
		result = CBooleanValue::Construct(runtime, false); // undefined != undefined is always false
		break;
	case t_boolean:
		result = CBooleanValue::Construct(runtime, lhs->AsBoolean() != rhs->AsBoolean());
		break;
	case t_int:
		result = CBooleanValue::Construct(runtime, lhs->AsInt() != rhs->AsInt());
		break;
	case t_uint:
		result = CBooleanValue::Construct(runtime, lhs->AsUInt() != rhs->AsUInt());
		break;
	case t_double:
		result = CBooleanValue::Construct(runtime, lhs->AsDouble() != rhs->AsDouble());
		break;
	case t_string:
		result = CBooleanValue::Construct(runtime, lhs->ToString() != rhs->ToString());
		break;
	case t_callable:
	case t_array:
	case t_object:
		result = CBooleanValue::Construct(runtime, lhs->AddressOf() != rhs->AddressOf());
		break;
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}

DEFINE_OPERATOR(OP_STRICT_EQUALITY)
{
	if(_lhs->Type() != _rhs->Type())
		return CBooleanValue::Construct(runtime, false);

	return OP_EQUALITY(runtime, _lhs, _rhs);
}
DEFINE_OPERATOR(OP_STRICT_UNEQUALITY)
{
	if (_lhs->Type() != _rhs->Type())
		return CBooleanValue::Construct(runtime, true);

	return OP_UNEQUALITY(runtime, _lhs, _rhs);
}

DEFINE_OPERATOR(OP_LOGICAL_AND)
{
	if(!_lhs->IsBooleanConvertible())
		throw CRuntimeError(runtime, std::format("\"{}\" is not convertible to a boolean", _lhs->TypeAsString()));

	if (!_rhs->IsBooleanConvertible())
		throw CRuntimeError(runtime, std::format("\"{}\" is not convertible to a boolean", _rhs->TypeAsString()));

	return CBooleanValue::Construct(runtime, _lhs->ToBoolean() && _rhs->ToBoolean());
}
DEFINE_OPERATOR(OP_LOGICAL_OR)
{
	if (!_lhs->IsBooleanConvertible())
		throw CRuntimeError(runtime, std::format("\"{}\" is not convertible to a boolean", _lhs->TypeAsString()));

	if (!_rhs->IsBooleanConvertible())
		throw CRuntimeError(runtime, std::format("\"{}\" is not convertible to a boolean", _rhs->TypeAsString()));

	return CBooleanValue::Construct(runtime, _lhs->ToBoolean() || _rhs->ToBoolean());
}
DEFINE_OPERATOR(OP_LEFT_SHIFT)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CIntValue::Construct(runtime, lhs->AsInt() << rhs->AsInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->AsUInt() << rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only shift int operands, had \"{}\" and \"{}\"", 
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_RIGHT_SHIFT)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CIntValue::Construct(runtime, lhs->AsInt() >> rhs->AsInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->AsUInt() >> rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_BITWISE_OR)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CIntValue::Construct(runtime, lhs->AsInt() | rhs->AsInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->AsUInt() | rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_BITWISE_XOR)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CIntValue::Construct(runtime, lhs->AsInt() ^ rhs->AsInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->AsUInt() ^ rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			lhs->TypeAsString(), rhs->TypeAsString()));
	}

	//was this allocated just now?
	if (alloc)
		alloc->Release();

	return result;
}
DEFINE_OPERATOR(OP_BITWISE_AND)
{
	auto [lhs, rhs, alloc] = Coerce(runtime, _lhs, _rhs);
	IValue* result{ nullptr };

	switch (lhs->Type()) {
	case t_int:
		result = CIntValue::Construct(runtime, lhs->AsInt() & rhs->AsInt());
		break;
	case t_uint:
		result = CUIntValue::Construct(runtime, lhs->AsUInt() & rhs->AsUInt());
		break;
	case t_undefined:
	case t_boolean:
	case t_double:
	case t_string:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
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
		throw CRuntimeError(lhs->GetAllocator(), "cannot assign to an immutable value");

	if (!lhs->GetOwner())
		throw CRuntimeError(lhs->GetAllocator(), "cannot assign to a temporary value");

	if (lhs->Type() != rhs->Type())
		throw CRuntimeError(lhs->GetAllocator(), std::format("can't assign \"{}\" to \"{}\"", rhs->TypeAsString(), lhs->TypeAsString()));
}

DEFINE_OPERATOR(OP_ASSIGNMENT_ADDITION)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() += _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() += _rhs->AsUInt();
		break;
	case t_double:
		_lhs->AsDouble() += _rhs->AsDouble();
		break;
	case t_string:
		_lhs->AsString() += _rhs->AsString();
		break;
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", _lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_SUBTRACTION)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() -= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() -= _rhs->AsUInt();
		break;
	case t_double:
		_lhs->AsDouble() -= _rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", _lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_MULTIPLICATION)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() *= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() *= _rhs->AsUInt();
		break;
	case t_double:
		_lhs->AsDouble() *= _rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", _lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_DIVISION)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:

		if (_rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsInt() /= _rhs->AsInt();

		break;
	case t_uint:

		if (_rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsUInt() /= _rhs->AsUInt();

		break;
	case t_double:

		if (_rhs->ToDouble() == 0.0)
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsDouble() /= _rhs->AsDouble();
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", _lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_MODULO)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:

		if (_rhs->ToInt() == VarjusInt(0))
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsInt() %= _rhs->AsInt();

		break;
	case t_uint:

		if (_rhs->ToUInt() == VarjusUInt(0))
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsUInt() %= _rhs->AsUInt();

		break;
	case t_double:

		if (_rhs->ToDouble() == 0.0)
			throw CRuntimeError(runtime, "division by 0");

		_lhs->AsDouble() = std::fmod(_lhs->AsDouble(), _rhs->AsDouble());
		break;
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("incompatible operands \"{}\" and \"{}\"", _lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_LEFT_SHIFT)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() <<= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() <<= _rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			_lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
DEFINE_OPERATOR(OP_ASSIGNMENT_RIGHT_SHIFT)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() >>= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() >>= _rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only shift int operands, had \"{}\" and \"{}\"",
			_lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;

}
DEFINE_OPERATOR(OP_ASSIGNMENT_BITWISE_OR)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() |= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() |= _rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			_lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;

}
DEFINE_OPERATOR(OP_ASSIGNMENT_BITWISE_XOR)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() ^= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() ^= _rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			_lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;

}
DEFINE_OPERATOR(OP_ASSIGNMENT_BITWISE_AND)
{
	OP_ASSIGNMENT_CHECK(_lhs, _rhs);

	switch (_lhs->Type()) {

	case t_int:
		_lhs->AsInt() &= _rhs->AsInt();
		break;
	case t_uint:
		_lhs->AsUInt() &= _rhs->AsUInt();
		break;
	case t_double:
	case t_string:
	case t_boolean: //unsafe
	case t_undefined:
	case t_callable:
	case t_array:
	case t_object:
		throw CRuntimeError(runtime, std::format("you can only use bitwise operations with int operands, had \"{}\" and \"{}\"",
			_lhs->TypeAsString(), _rhs->TypeAsString()));
	}

	return _lhs;
}
