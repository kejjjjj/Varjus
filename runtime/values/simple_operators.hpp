#pragma once

class IValue;
class CVariable;

IValue* OP_ASSIGNMENT(CVariable* lhs, IValue* rhs);
IValue* OP_ADDITION(IValue* lhs, IValue* rhs);
