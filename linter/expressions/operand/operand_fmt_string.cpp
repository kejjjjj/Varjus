#include "operand_fmt_string.hpp"
#include "operand.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/tokenizer.hpp"
#include "linter/punctuation.hpp"

#include <ranges>
#include <cassert>

#define FMT_STRING_CHAR '$'
#define FMT_EXPRESSION_CHAR '('
#define FMT_EXPRESSION_PUNC p_par_open

CFmtStringOperand::CFmtStringOperand(VectorOf<FmtStringAST>&& nodes)
	: m_oNodes(std::move(nodes)){}

CFmtStringOperand::~CFmtStringOperand() = default;

std::unique_ptr<IOperand> CLinterOperand::ParseFormatString()
{
	const auto& parseString = (*m_iterPos)->Source();
	auto& oldIter = m_iterPos;

	std::string raw;

	VectorOf<FmtStringAST> fmtString;

	for (auto it = parseString.begin(); it != parseString.end(); ++it) {

		while (it != parseString.end() && *it != FMT_STRING_CHAR) {
			raw.push_back(*it);
			std::advance(it, 1);
		}

		if (it == parseString.end())
			break;

		std::advance(it, 1); // skip the fmt char

		if (it == parseString.end()) {
			raw.push_back(*it);
			std::advance(it, 1);
			break;
		}

		assert(it != parseString.end());

		if (*it == FMT_EXPRESSION_CHAR) {

			if (raw.size()) {
				fmtString.emplace_back(FmtStringAST(raw));
				raw.clear();
			}
			
			std::advance(it, 1); // skip (

			//tokenize everything until the end
			auto str = std::string(it, parseString.end());
			CBufferTokenizer tokenizer(str);

			if (!tokenizer.Tokenize()) {
				CLinterErrors::PushError("format string tokenization failure", GetIteratorSafe()->m_oSourcePosition);
				return nullptr;
			}

			auto tokens = tokenizer.GetTokens();
			std::size_t len{};
			for (const auto& t : tokens)
				len += t->Source().length();

			auto begin = tokens.begin();
			auto end = tokens.end();

			CLinterExpression expr(begin, end, m_pScope, m_pOwner);

			if (!expr.Parse(PairMatcher(FMT_EXPRESSION_PUNC)))
				return nullptr;

			fmtString.emplace_back(FmtStringAST(expr.ToMergedAST()));
			std::advance(it, len-1);
			raw.clear();
		}
	}

	if (raw.size()) {
		fmtString.emplace_back(FmtStringAST(raw));
		raw.clear();
	}
	std::advance(m_iterPos, 1);
	auto ptr = std::make_unique<CFmtStringOperand>(std::move(fmtString));
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
}

ASTNode CFmtStringOperand::ToAST() {
	return std::make_shared<FmtStringASTNode>(m_oCodePosition, std::move(m_oNodes));
}
