#pragma once

#include "declarations/members.hpp"

struct CProgramContext
{
	CStringIntegerHashMap m_oAllMembers;
	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;

	void Reset() {
		m_oAllMembers.Clear();
		m_oAllVariables.Clear();
		m_oAllGlobalVariables.Clear();
	}

};
