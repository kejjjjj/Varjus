#pragma once

#include "structure.hpp"

class CProgramRuntime
{
	NONCOPYABLE(CProgramRuntime);
public:
	// only one file for now
	CProgramRuntime(CFileRuntimeData* const file);
	~CProgramRuntime();

	void Execute();

private:

	std::vector<RuntimeFunction> m_oFunctions;
};
