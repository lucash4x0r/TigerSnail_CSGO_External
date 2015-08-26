#ifndef _KEYS_H
#define _KEYS_H

#include "includes.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: keys.h: Key manager interface
*/

//======================================================================
#define INVALID_KEY_IDENT std::string::npos
//======================================================================

//======================================================================
struct keyinfo_s {
	std::string szName;
	int iKeyCode;
};
//======================================================================

//======================================================================
class CKeyManager {
private:
	std::vector<keyinfo_s> m_vKeys;
public:
	CKeyManager() {}
	~CKeyManager() { this->Clear(); }

	bool AddKey(const std::string& szName, int iKeyCode);

	bool KeyExists(const std::string& szName, size_t* puiEntryId);
	bool KeyExists(int iKeyCode, size_t* puiEntryId);

	int GetKeyNum(const std::string& szKeyName);
	std::string GetKeyName(int ivKeyCode);

	const size_t GetAmountOfKeys(void);
	keyinfo_s* GetKeyById(const size_t uiId);

	void Clear(void);
};
//======================================================================

#endif
