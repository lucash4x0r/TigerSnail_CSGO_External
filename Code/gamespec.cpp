#include "gamespec.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: gamespec.cpp: Game specific implementations
*/

//======================================================================
client_interface_export* CGameSpecific::FindExportInterfaceRegsEx(LPVOID lpvpfnCreateInterfaceOfClientDll)
{
	//Get start address to the value of InterfaceReg::s_pInterfaceRegs.
	//It points to the first entry in the linked list of client.dll exports

	if (!lpvpfnCreateInterfaceOfClientDll)
		return NULL;

	DWORD dwDestAddr;
	DWORD dwDWord;
	
	if (!ReadFromProcess((LPCVOID)((DWORD)lpvpfnCreateInterfaceOfClientDll + 0x05), &dwDWord, sizeof(DWORD))) //Get to address of jump instruction
		return NULL;

	dwDestAddr = ((DWORD)lpvpfnCreateInterfaceOfClientDll + 0x04) + dwDWord + 0x05; //Calculate absolute destination address
	
	if (!ReadFromProcess((LPCVOID)(dwDestAddr + 0x06), &dwDWord, sizeof(DWORD))) //Get pointer to InterfaceReg::s_pInterfaceRegs
		return NULL;
	
	if (!ReadFromProcess((LPCVOID)(dwDWord), &dwDWord, sizeof(DWORD))) //Copy the address value of InterfaceReg::s_pInterfaceRegs
		return NULL;

	return (client_interface_export*)dwDWord;
}
//======================================================================

//======================================================================
LPVOID CGameSpecific::FindClientExportEx(LPVOID lpvpfnCreateInterfaceOfClientDll, const std::string& szExportName)
{
	//Walk trough the linked list of client.dll exports and find the desired one's function pointer.
	//The function the pointer points to can be called to get the pointer of a client.dll class which
	//is exported to the engine to call the functions (as I think)

	if (!szExportName.length())
		return NULL;

	static client_interface_export* pClientExport = NULL; //Pointer to the first entry in list

	if (!pClientExport) {
		if (!lpvpfnCreateInterfaceOfClientDll)
			return NULL;

		//Get pointer to the first entry in the linked list
		pClientExport = FindExportInterfaceRegsEx(lpvpfnCreateInterfaceOfClientDll);
		if (!pClientExport)
			return NULL;
	} 
	
	if (pClientExport) {
		client_interface_export CurrentEntry = {0};
		char szNameStr[MAX_INTERFACE_NAMELEN + 1] = {0};

		//Get first entry in linked list
		if (!ReadFromProcess(pClientExport, &CurrentEntry, sizeof(CurrentEntry)))
			return NULL;

		do { //Loop...
			//Read string of current entry
			if (!ReadFromProcess(CurrentEntry.pszExportName, szNameStr, sizeof(szNameStr)))
				return NULL;

			szNameStr[MAX_INTERFACE_NAMELEN] = 0; //Just to be sure we won't step over the line ;-)
			
			 //Return the function pointer if entry by name has been found
			if (szExportName == szNameStr)
				return CurrentEntry.lpvFunction;
				
			//Goto next entry
			if ((CurrentEntry.pNextExport) && (!ReadFromProcess(CurrentEntry.pNextExport, &CurrentEntry, sizeof(CurrentEntry))))
				return NULL;
				
		} while (CurrentEntry.pNextExport); //... while the end is not reached
	}

	return NULL;
}
//======================================================================

//======================================================================
CClientEntityList* CGameSpecific::GetCClientEntityListPtrEx(LPVOID lpvpfnInterfaceFnctn)
{
	//777DBBA0   B8 6438DC77      MOV EAX,client.77DC3864
	//777DBBA5   C3               RETN

	if (!lpvpfnInterfaceFnctn)
		return NULL;

	CClientEntityList* pResult;

	if (!ReadFromProcess((LPCVOID)((DWORD)lpvpfnInterfaceFnctn + 0x01), &pResult, sizeof(pResult)))
		return NULL;

	return pResult;
}
//======================================================================

//======================================================================
VMatrix* CGameSpecific::FindW2SMatrix(DWORD dwEngineDllBase)
{
	//Find engines' world-2-screen matrix

	if (!dwEngineDllBase)
		return NULL;

	//Find offset to conditional value and multiplier
	DWORD dwOfsCondMult = GetOffset("W2SMatrix::CondMult");
	if (!dwOfsCondMult)
		return NULL;

	DWORD dwValue;

	//Read out address
	if (!ReadFromProcess((LPCVOID)(dwEngineDllBase + dwOfsCondMult), &dwValue, sizeof(DWORD)))
		return NULL;
	
	//Read out value
	if (!ReadFromProcess((LPCVOID)dwValue, &dwValue, sizeof(DWORD)))
		return NULL;
	
	VMatrix* pResult = NULL;

	if (dwValue > 1) { //Check value according to listing
		DWORD dwOfsMatrix1 = GetOffset("W2SMatrix::Matrix1");
		if (!dwOfsMatrix1)
			return NULL;

		DWORD dwValueBackup = dwValue;

		//Read out address to pointer
		if (!ReadFromProcess((LPCVOID)(dwEngineDllBase + dwOfsMatrix1), &dwValue, sizeof(DWORD)))
			return NULL;
		
		//Read out pointer value
		if (!ReadFromProcess((LPCVOID)dwValue, &dwValue, sizeof(DWORD)))
			return NULL;

		//Perform calculations according to listing and save the result
		pResult = (VMatrix*)(dwValue - 0x44 + dwValueBackup * 0x210);
	} else {
		DWORD dwOfsMatrix2 = GetOffset("W2SMatrix::Matrix2");
		if (!dwOfsMatrix2)
			return NULL;

		//Read out address
		if (!ReadFromProcess((LPCVOID)(dwEngineDllBase + dwOfsMatrix2), &pResult, sizeof(DWORD)))
			return NULL;
	}
	
	return pResult;
}
//======================================================================

//======================================================================
bool CGameSpecific::Initialize(const std::string& szProcName)
{
	//Initialize the component
	
	if (this->m_bReady)
		return TRUE;
	
	this->m_bReady = false;

	if (!szProcName.length())
		return false;

	//Get process ID
	if (!GetProcessId(szProcName.c_str()))
		return false;
	
	this->m_pPE = ProcessEntry();
	if (!this->m_pPE)
		return false;

	//Open handle to process
	if (!OpenProcess(this->m_pPE->th32ProcessID, PROCESS_ALL_ACCESS))
		return false;

	//Register modules
	#define GET_MODULE_BASE(n) if (!GetModuleBase(n)) return false;
	GET_MODULE_BASE("client.dll");
	GET_MODULE_BASE("engine.dll");

	//Get module base for client.dll
	DWORD dwClientDll = GetModuleHandleByName("client.dll");
	if (!dwClientDll)
		return false;

	//Get offset for function
	DWORD dwOfsFnctn = GetOffset("CreateInterface");
	if (!dwOfsFnctn)
		return false;

	//Find interface function for the VCLIENTENTITYLIST_INTERFACE_VERSION interface object
	LPVOID pInterfaceFnctn = FindClientExportEx((LPVOID)(dwClientDll + dwOfsFnctn), VCLIENTENTITYLIST_INTERFACE_VERSION);
	if (!pInterfaceFnctn)
		return false;
	
	//Get start address of CClientEntityList object instance
	this->m_pClientEntityList = GetCClientEntityListPtrEx(pInterfaceFnctn);
	if (!this->m_pClientEntityList)
		return false;

	//Get offset to array member
	DWORD dwOfsArray = GetOffset("CClientEntityList::m_EntityCacheInfo");
	if (!dwOfsArray)
		return false;
	
	//Get address of entity list array
	this->m_pEntityList = (HENTITY*)((DWORD)this->m_pClientEntityList + dwOfsArray);

	//Get offset to pointer
	DWORD dwOfsGameResourcePtr = GetOffset("C_PlayerResource Ptr");
	if (!dwOfsGameResourcePtr)
		return false;

	//Get offset to array member
	DWORD dwOfsNameListArray = GetOffset("C_PlayerResource::m_szName");
	if (!dwOfsNameListArray)
		return false;

	DWORD dwAddress;

	//Read out pointer value to object instane
	if (!ReadFromProcess((LPCVOID)(dwClientDll + dwOfsGameResourcePtr), &dwAddress, sizeof(DWORD)))
		return false;

	//Read out object instance base address
	if (!ReadFromProcess((LPCVOID)dwAddress, &dwAddress, sizeof(DWORD)))
		return false;

	//Calculate and save absolute address to the name string table
	this->m_pNameList = (string_t*)(dwAddress + dwOfsNameListArray);

	//Get base address of engine.dll
	DWORD dwEngineBaseAddr = GetModuleHandleByName("engine.dll");
	if (!dwEngineBaseAddr)
		return false;

	//Get VMatrix pointer of W2S Matrix
	this->m_pW2SMatrix = FindW2SMatrix(dwEngineBaseAddr);
	if (!this->m_pW2SMatrix)
		return false;

	//Calculate address to player related object instance
	DWORD dwOfsObjectPtr = GetOffset("LocalPlayer::Pointer");
	if (!dwOfsObjectPtr)
		return false;

	//Read out pointer value
	if (!ReadFromProcess((LPCVOID)(dwEngineBaseAddr + dwOfsObjectPtr), &this->m_pLocalPlayerAddr, sizeof(DWORD)))
		return false;

	//Read out object instance address
	if (!ReadFromProcess((LPCVOID)this->m_pLocalPlayerAddr, &this->m_pLocalPlayerAddr, sizeof(DWORD)))
		return false;

	return this->m_bReady = true;
}
//======================================================================

//======================================================================
bool CGameSpecific::Clear(void)
{
	//Clear component usage

	if (!this->m_bReady)
		return false;

	//Close process
	return CloseProcess();
}
//======================================================================

//======================================================================
bool CGameSpecific::AddOffset(const std::string& szName, DWORD dwOffset)
{
	//Pass to COffsetMgr::AddOffset

	return COffsetMgr::AddOffset(szName, dwOffset);
}
//======================================================================

//======================================================================
void CGameSpecific::ClearOffsets(void)
{
	//Pass to COffsetMgr::Clear

	COffsetMgr::Clear();
}
//======================================================================

//======================================================================
int CGameSpecific::GetMaxEntities(void)
{
	//Get maximum entity slot amount

	if (!this->m_bReady)
		return 0;

	return NUM_ENT_ENTRIES;
}
//======================================================================

//======================================================================
int CGameSpecific::GetMaxClients(void)
{
	//Get maximum amount of player slots

	if (!this->m_bReady)
		return 0;

	return MAX_CLIENTS;
}
//======================================================================

//======================================================================
HENTITY CGameSpecific::GetEntityByIndex(int iIndex)
{
	//Get entity data pointer by ID

	if (!this->m_bReady)
		return NULL;

	//Check index size
	if ((iIndex < 0) || (iIndex > GetMaxEntities()))
		return NULL;

	HENTITY hEntity;

	//Read out entity handle from array
	if (!ReadFromProcess((LPCVOID)((DWORD)this->m_pEntityList + ((DWORD)iIndex * 0x08)), &hEntity, sizeof(HENTITY))) 
		return NULL;

	//Return pointer to entity object instance
	return hEntity;
}
//======================================================================

//======================================================================
bool CGameSpecific::GetEntityInfoByHandle(const HENTITY hEntity, entity_info_s* pInfoOut)
{
	//Read out entity info of given entity

	if ((!hEntity) || (!pInfoOut))
		return false;

	//Calculate C_BaseEntity object base address
	register DWORD dwBaseEntAddr = hEntity - 0x08;

	//Read actual data of this entity
	return ReadFromProcess((LPCVOID)dwBaseEntAddr, pInfoOut, sizeof(entity_info_s));
}
//======================================================================

//======================================================================
std::string CGameSpecific::GetPlayerName(const int iPlayerIndex)
{
	//Read out player name and return the string object

	DWORD dwStringAddr;

	//Read out string address for this player entity
	if (!ReadFromProcess((LPCVOID)((DWORD)this->m_pNameList + (DWORD)(iPlayerIndex - 1) * sizeof(string_t)), &dwStringAddr, sizeof(string_t)))
		return "";

	char szName[MAX_PLAYER_NAMELEN] = {0};

	//Read out name string
	if (!ReadFromProcess((LPCVOID)dwStringAddr, szName, sizeof(szName)))
		return "";

	return std::string(szName);
}
//======================================================================

//======================================================================
bool CGameSpecific::IsEntityPlaying(const entity_info_s& rEntityInfo)
{
	//Indicate whether this entity is a player and is 'alive'
	
	return ((rEntityInfo.m_iIndex > 0) && (rEntityInfo.m_iIndex <= 64)) && (rEntityInfo.m_iTeamNum > TEAM_SPECTATOR) && (!rEntityInfo.m_bDormant) && (rEntityInfo.m_iHealth > 0) && (rEntityInfo.m_lifeState == LIFE_ALIVE);
}
//======================================================================

//======================================================================
VMatrix* CGameSpecific::WorldToScreenMatrix(void)
{
	//Return pointer to world-2-screen matrix

	return this->m_pW2SMatrix;
}
//======================================================================

//======================================================================
bool CGameSpecific::WorldToScreenMatrix(w2smatrix_t* pvMatrixOut)
{
	//Get world-2-screen matrix data

	if (!pvMatrixOut)
		return false;

	w2smatrix_t vMatrix;

	if (!ReadFromProcess(this->m_pW2SMatrix, &vMatrix, sizeof(VMatrix)))
		return false;

	memcpy(pvMatrixOut, vMatrix, sizeof(w2smatrix_t));

	return true;
}
//======================================================================

//======================================================================
int CGameSpecific::GetLocalPlayerIndex(void)
{
	//Get index value of local player

	int iIndex;

	if (!ReadFromProcess((LPCVOID)((DWORD)this->m_pLocalPlayerAddr + 0x160), &iIndex, sizeof(int)))
		return 0;

	return iIndex + 1;
}
//======================================================================
