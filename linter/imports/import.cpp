#include "import.hpp"
#include "globalEnums.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"
#include "linter/linter.hpp"
#include "fs/fs_globals.hpp"

#include <cassert>

CImportLinter::CImportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {}

Success CImportLinter::Parse()
{

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_import) {
		CLinterErrors::PushError("expected \"import\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (!ParseIdentifierRecursively())
		return failure;

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_from) {
		CLinterErrors::PushError("expected \"from\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); // skip from

	if (!ParseFilePath())
		return failure;

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_semicolon)) {
		CLinterErrors::PushError("expected \";\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); // skip ;

	return success;
}

Success CImportLinter::ParseIdentifierRecursively()
{

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		CLinterErrors::PushError("expected a name", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (m_pOwner->GetContext()->m_sFilePath.empty())
		CLinterErrors::PushError("imports are not supported when there is no working directory", 
			GetIteratorSafe()->m_oSourcePosition);

	m_oNames.push_back((*m_iterPos)->Source());
	std::advance(m_iterPos, 1);
	
	if (IsEndOfBuffer() || (*m_iterPos)->Type() == tt_from || !(*m_iterPos)->IsOperator(p_comma)) {
		return success;
	}

	std::advance(m_iterPos, 1); // skip ,
	return ParseIdentifierRecursively();
}

Success CImportLinter::ParseFilePath()
{
	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_string) {
		CLinterErrors::PushError("expected a string", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	const auto& relativePath = (*m_iterPos)->Source();
	const auto& wd = fs::previous_directory( m_pOwner->GetContext()->m_sFilePath );

	const auto fullPath = wd + '\\' + relativePath;

	if (!fs::file_exists(fullPath)) {
		CLinterErrors::PushError(std::format("\"{}\" does not exist", fullPath), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oTargetFile = fullPath;

	std::advance(m_iterPos, 1); // skip file name
	return success;
}

Success CImportLinter::ParseFile()
{
	CFileLinter l(m_iterPos, m_iterEnd, m_oTargetFile);

	if (!l.ParseFile())
		return failure;

	return success;

}
