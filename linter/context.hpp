#pragma once

#include "declarations/members.hpp"

struct CProgramContext
{
	CStringIntegerHashMap m_oAllMembers;
	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;
};
