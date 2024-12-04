#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/values/types/array_internal/array_builtin.hpp"

#include "runtime/values/simple_operators.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>
#include <format>

IValue* CRuntimeExpression::EvaluateUnary(CFunction* const thisFunction, const UnaryASTNode* node)
{
	auto operand = Evaluate(thisFunction, node->left.get());
	
	if (!operand->HasOwner())
		throw CRuntimeError("the unary operand is not a variable");

	if (node->IsIncrement()) {

		if(operand->Type() != t_int)
			throw CRuntimeError("the ++ operand must have an int type");

		++operand->AsInt();
	}
	
	return operand;
}
