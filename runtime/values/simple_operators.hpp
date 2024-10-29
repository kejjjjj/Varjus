#pragma once

class IValue;
class CVariable;

void OP_ASSIGNMENT(IValue* lhs, IValue* rhs);
IValue* OP_ADDITION(IValue* lhs, IValue* rhs);
IValue* OP_LESS_THAN(IValue* lhs, IValue* rhs);
