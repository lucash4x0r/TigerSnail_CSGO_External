#ifndef _OFFSETMGR_H
#define _OFFSETMGR_H

#include "includes.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: offsetmgr.h: Offset manager interface
*/

//======================================================================
struct offset_s {
	std::string szName;
	DWORD dwOffset;
};
//======================================================================

//======================================================================
class COffsetMgr {
private:
	std::vector<offset_s> m_vOffsets;

	inline bool OffsetExists(const std::string& szOffsetName);
protected:
	COffsetMgr() { }
	~COffsetMgr() { this->Clear(); }

	bool AddOffset(const std::string& szName, DWORD dwOffset);
	DWORD GetOffset(const std::string& szName);

	void Clear(void);
};
//======================================================================

#endif
