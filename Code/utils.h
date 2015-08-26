#ifndef _UTILS_H
#define _UTILS_H

#include "includes.h"
#include "gamespec.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: utils.h: Utility interface
*/

//======================================================================
#define CONSOLE_ATTRIBUTE_DEFAULT 0
#define FOREGROUND_PINK 13
//======================================================================

//======================================================================
class CTempPlayerData {
public:
	enum team_e {
		team_blue,
		team_red
	};

	struct temp_player_data_s {
		std::string szName;
		entity_info_s sEntInfo;
		team_e eTeam;
		bool bHasUpdatedOnce;
		bool bIsLocalPlayer;
		float fDistanceFromLocalPlayer;
		Vector vScreenPos;
		bool bScreenPosValid;
	};
private:

	std::vector<temp_player_data_s> m_vPlayerData;
public:
	CTempPlayerData() {}
	CTempPlayerData(const size_t uiSlots) { this->Setup(uiSlots); }
	~CTempPlayerData() { this->Clear(); }

	void Setup(const size_t uiSlots)
	{
		//Set array slots of list

		this->m_vPlayerData.resize(uiSlots);
	}

	void Clear(void)
	{
		//Clear list

		this->m_vPlayerData.clear();
	}

	bool UpdateSlot(const size_t uiSlot, const std::string& szName, const entity_info_s* pEntityInfo, const float fDistanceFromLocalPlayer, const bool bIsLocalPlayer = false)
	{
		//Update slot data

		if (uiSlot >= this->m_vPlayerData.size())
			return false;

		this->m_vPlayerData[uiSlot].szName = szName;
		this->m_vPlayerData[uiSlot].sEntInfo = *pEntityInfo;
		this->m_vPlayerData[uiSlot].bIsLocalPlayer = bIsLocalPlayer;

		if (this->m_vPlayerData[uiSlot].sEntInfo.m_iTeamNum > TEAM_SPECTATOR) {
			this->m_vPlayerData[uiSlot].eTeam = (this->m_vPlayerData[uiSlot].sEntInfo.m_iTeamNum == 2) ? team_red : team_blue;
		}

		if (!bIsLocalPlayer)
			this->m_vPlayerData[uiSlot].fDistanceFromLocalPlayer = fDistanceFromLocalPlayer;

		if (!this->m_vPlayerData[uiSlot].bHasUpdatedOnce)
			this->m_vPlayerData[uiSlot].bHasUpdatedOnce = true;

		return true;
	}

	bool UpdateSlot(const size_t uiSlot, const Vector& vScreenPos, const bool bScreenPosValid)
	{
		//Update slots screenpos data

		if (uiSlot >= this->m_vPlayerData.size())
			return false;

		this->m_vPlayerData[uiSlot].bScreenPosValid = bScreenPosValid;
		
		if (bScreenPosValid)
			this->m_vPlayerData[uiSlot].vScreenPos = vScreenPos;

		if (!this->m_vPlayerData[uiSlot].bHasUpdatedOnce)
			this->m_vPlayerData[uiSlot].bHasUpdatedOnce = true;

		return true;
	}

	temp_player_data_s* FromSlot(const size_t uiSlot)
	{
		//Get pointer to slot data from given slot

		if (uiSlot >= this->m_vPlayerData.size())
			return NULL;

		return &this->m_vPlayerData[uiSlot];
	}

	const size_t Size(void)
	{
		//Get slot size
		
		return this->m_vPlayerData.size();
	}
};
//======================================================================

//======================================================================
void PrintConsole(WORD wAttr, const char* lpszFmt, ...);
void FatalError(const char* lpszFmt, ...);

bool GetAbsoluteBasePath(std::string& szOut);

DWORD HexToSignedInt(const std::string& szHexStr);

bool IsValidWindow(HWND hWindow);

float GetDistance(const Vector& vSource, const Vector& vDest);

void DrawCursor(void);
//======================================================================

#endif
