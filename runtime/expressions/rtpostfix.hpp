#pragma once

class IValue;
class CFunction;
class SubscriptASTNode;

[[nodiscard]] IValue* EvaluateSubscript(CFunction* const thisFunction, const SubscriptASTNode* node);
