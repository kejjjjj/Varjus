#pragma once

class IConstEvalValue;

IConstEvalValue* OPT_ASSIGN(IConstEvalValue* lhs, IConstEvalValue* rhs);
IConstEvalValue* OPT_ADDITION(IConstEvalValue* lhs, IConstEvalValue* rhs);
