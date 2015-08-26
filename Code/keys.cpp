#include "keys.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: keys.cpp: General key manager implementation
*/

//======================================================================
bool CKeyManager::KeyExists(const std::string& szName, size_t* puiEntryId)
{
	//Check if key entry exists by name

	if (!szName.length())
		return false;

	//Search in list
	for (size_t i = 0; i < this->m_vKeys.size(); i++) {
		if (this->m_vKeys[i].szName == szName) { //Compare name string
			if (puiEntryId) //Set entry ID if desired
				*puiEntryId = i;

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
bool CKeyManager::KeyExists(int iKeyCode, size_t* puiEntryId)
{
	//Check if key entry exists by virtual key code

	if (!iKeyCode)
		return FALSE;

	//Search in list
	for (size_t i = 0; i < this->m_vKeys.size(); i++) {
		if (this->m_vKeys[i].iKeyCode == iKeyCode) {
			if (puiEntryId) //Set entry ID if desired
				*puiEntryId = i;

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
bool CKeyManager::AddKey(const std::string& szName, int iKeyCode)
{
	//Add new key info

	if ((!szName.length()) || (!iKeyCode))
		return false;

	//Check for limit
	if (m_vKeys.size()+1 == INVALID_KEY_IDENT)
		return false;

	//Check if already exists
	if (this->KeyExists(szName, NULL))
		return false;

	//Add to list
	keyinfo_s ki;
	ki.szName = szName;
	ki.iKeyCode = iKeyCode;
	this->m_vKeys.push_back(ki);

	return true;
}
//======================================================================

//======================================================================
int CKeyManager::GetKeyNum(const std::string& szKeyName)
{
	//Get virtual key code by key name

	if (!szKeyName.length())
		return 0;

	//Search entry
	size_t dwEntryId;
	if (!this->KeyExists(szKeyName, &dwEntryId))
		return 0;

	return this->m_vKeys[dwEntryId].iKeyCode;
}
//======================================================================

//======================================================================
std::string CKeyManager::GetKeyName(int iKeyCode)
{
	//Get key name string by virtual key code

	if (!iKeyCode)
		return "";

	//Search entry
	size_t dwEntryId;
	if (!this->KeyExists(iKeyCode, &dwEntryId))
		return "";

	return this->m_vKeys[dwEntryId].szName;
}
//======================================================================

//======================================================================
const size_t CKeyManager::GetAmountOfKeys(void)
{
	//Get amount of registered keys

	return this->m_vKeys.size();
}
//======================================================================

//======================================================================
keyinfo_s* CKeyManager::GetKeyById(const size_t uiId)
{
	//Get key info data by ID

	if (uiId < this->m_vKeys.size())
		return &this->m_vKeys[uiId];

	return NULL;
}
//======================================================================

//======================================================================
void CKeyManager::Clear(void)
{
	//Clear entries

	this->m_vKeys.clear();
}
//======================================================================
