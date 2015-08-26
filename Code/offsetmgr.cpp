#include "offsetmgr.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: offsetmgr.cpp: Offset manager implementation
*/

//======================================================================
inline bool COffsetMgr::OffsetExists(const std::string& szOffsetName)
{
	//Check if offset already exists

	return this->GetOffset(szOffsetName) != 0;
}
//======================================================================

//======================================================================
bool COffsetMgr::AddOffset(const std::string& szName, DWORD dwOffset)
{
	//Add new offset entry

	if (!szName.length())
		return false;

	//Check if already exists
	if (this->OffsetExists(szName))
		return false;

	//Set data and add to list
	offset_s ofs;
	ofs.szName = szName;
	ofs.dwOffset = dwOffset;
	this->m_vOffsets.push_back(ofs);

	return true;
}
//======================================================================

//======================================================================
DWORD COffsetMgr::GetOffset(const std::string& szName)
{
	//Get offset value by name

	if (!szName.length())
		return 0;

	//Search in list
	for (size_t i = 0; i < this->m_vOffsets.size(); i++) {
		if (this->m_vOffsets[i].szName == szName)
			return this->m_vOffsets[i].dwOffset;
	}

	return 0;
}
//======================================================================

//======================================================================
void COffsetMgr::Clear(void)
{
	//Clear list

	this->m_vOffsets.clear();
}
//======================================================================
