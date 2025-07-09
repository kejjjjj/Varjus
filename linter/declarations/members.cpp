#include "members.hpp"

std::size_t CStringIntegerHashMap::operator[](const VarjusString& key){
	return Insert(key);
}
[[maybe_unused]] std::size_t CStringIntegerHashMap::Insert(const VarjusString& key)
{
	const auto index = m_oStringToId.find(key);

	if (index == m_oStringToId.end()) {
		m_oStringToId[key] = GetCurrentIndex();
		m_oIdToString[m_oStringToId[key]] = key;
	}

	return m_oStringToId[key];
}
void CStringIntegerHashMap::Erase(const VarjusString& key)
{
	if (!Contains(key))
		return;

	auto id = At(key);

	m_oStringToId.erase(key);
	m_oIdToString.erase(id);

}
bool CStringIntegerHashMap::Contains(const VarjusString& key) const{
	return m_oStringToId.contains(key);
}
const VarjusString& CStringIntegerHashMap::At(const __GlobalMemberIndex key) const{
	return m_oIdToString.at(key);
}
__GlobalMemberIndex CStringIntegerHashMap::At(const VarjusString& key) const{
	return m_oStringToId.at(key);
}
std::size_t CStringIntegerHashMap::GetCurrentIndex() const
{
	 return m_oStringToId.size();
}
