#include "members.hpp"

std::size_t CStringIntegerHashMap::operator[](const std::string& key){
	return Insert(key);
}
[[maybe_unused]] std::size_t CStringIntegerHashMap::Insert(const std::string& key)
{
	const auto index = m_oStringToId.find(key);

	if (index == m_oStringToId.end()) {
		m_oStringToId[key] = GetCurrentIndex();
		m_oIdToString[m_oStringToId[key]] = key;
	}

	return m_oStringToId[key];
}

bool CStringIntegerHashMap::Contains(const std::string& key) const{
	return m_oStringToId.contains(key);
}
const std::string& CStringIntegerHashMap::At(const GlobalMemberIndex key) const{
	return m_oIdToString.at(key);
}
GlobalMemberIndex CStringIntegerHashMap::At(const std::string& key) const{
	return m_oStringToId.at(key);
}
std::size_t CStringIntegerHashMap::GetCurrentIndex() const
{
	 return m_oStringToId.size();
}
