
#pragma once

#include "expressions/definitions.hpp"

class CFileLinter : public CLinter<CToken>
{
public:
	CFileLinter() = delete;
	CFileLinter(LinterIterator& start, LinterIterator& end);
	CFileLinter operator=(const CFileLinter&) = delete;

	[[nodiscard]] Success ParseFile();
};