#ifndef _GAMESPEC_H
#define _GAMESPEC_H

#include "includes.h"
#include "process.h"
#include "offsetmgr.h"

#define CLIENT_DLL //Use client related SDK content. This must be defined before including SDK files.

#include <cbase.h>
#include <vgui\ISurface.h>
#include <vgui\IPanel.h>

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: gamespec.h: Game specific interface
*/

/*
	The client.dll module provides an interface function called CreateInterface.
	It is exported via the export address table of the module. Its purpose is to
	look for own exported interfaces to the engine. The identity of the interface 
	is passed via a string (for instance VClient017). The CreateInterface function
	now searches for the string in a linked list. If found it returns the address
	of a function. This function now simply returns the start address of a class
	object with API functions. Those functions are called by the engine. You can
	also pass a pointer to an integer to the CreateInterface function which will
	be set to a status value (either success or failure) but it is not really 
	important, so you just can pass NULL. The client.dll can now define the game
	mod via the exports and the imports from the engine as well.

	//Entry Point of:
	//void* __cdecl CreateInterface(const char*, int*);
	5487D700 >|$ 55             |PUSH EBP								 ; Push EBP onto stack to save old frame pointer
	5487D701  |. 8BEC           |MOV EBP,ESP							 ; Copy ESP to EBP to have frame pointer for this function
	5487D703  |. 5D             |POP EBP							     ; Restore old frame pointer 
	5487D704  \.^E9 87FFFFFF    \JMP client.5487D690					 ; Jump to code part below
	->
	5487D690  /> 55             /PUSH EBP
	5487D691  |. 8BEC           |MOV EBP,ESP
	5487D693  |. 56             |PUSH ESI
	5487D694  |. 8B35 E8ABD554  |MOV ESI,DWORD PTR DS:[54D5ABE8]         ;  Copy address of 'InterfaceReg::s_pInterfaceRegs' to ESI

	To see how the client is handling the stuff just have a look at the Source SDK.
	
	Well, the class CClientEntityList is managing a list of all possible entities. You can see how it accesses the array named m_EntityCacheInfo
	of type EntityCacheInfo_t via the disassembly of its method 'GetClientNetworkable' (currently vft index 0).

	Disassembly of IClientEntityList::GetClientNetworkable() method
	536ADB00   55               PUSH EBP	; Backup EBP on stack
	536ADB01   8BEC             MOV EBP,ESP	; Copy ESP to EBP, stackpointer setup done
	536ADB03   8B45 08          MOV EAX,DWORD PTR SS:[EBP+8]	; Copy method argument (int entnum) to EAX
	536ADB06   8B44C1 28        MOV EAX,DWORD PTR DS:[ECX+EAX*8+28]	; Copy element value of this::m_EntityCacheInfo[entnum].m_pNetworkable to EAX as result
	536ADB0A   5D               POP EBP	; Restore old EBP from stack
	536ADB0B   C2 0400          RETN 4	; Restore return address and free argument space from stack

	If you now have the value of m_pNetworkable then you just need to subtract 0x08 bytes. Now you have the base address of the C_BaseEntity object
	instance of this entity. You can basically handle it as a struct and read it out completely and then access the values. To access player names
	you can access the C_PlayerResource object with its array named m_szName which holds addresses of the name string im memory.

	Access player name strings via C_PlayerResource::m_szName[<max clients value>]; it's a string pointer array (string_t)
	client.dll + 0x241E0  /$ 55            PUSH EBP	; Start address of C_PlayerResource::UpdatePlayerName() method; Backup EBP onto stack
	520741E1  |. 8BEC                      MOV EBP,ESP	; Copy value of ESP to EBP. Frame pointer setup done
	520741E3  |. 81EC 58010000             SUB ESP,158	; Subtract value from ESP to allocate stack variable memory
	520741E9  |. 56                        PUSH ESI	; Backup ESI on stack
	520741EA  |. 8B75 08                   MOV ESI,DWORD PTR SS:[EBP+8]	; Copy DWORD at [EBP+0x08] (function argument, player slot) to ESI
	520741ED  |. 8D46 FF                   LEA EAX,DWORD PTR DS:[ESI-1]	; Load expression [ESI-1] to EAX
	520741F0  |. 57                        PUSH EDI	; Backup EDI on stack
	520741F1  |. 8BF9                      MOV EDI,ECX	; Copy ECX (class instance this pointer) to EDI
	520741F3  |. 83F8 3F                   CMP EAX,3F	; Compare EAX with 0x3F (63; ESI-1; MAX_PLAYER_SLOTS-1)
	520741F6  |. 77 6D                     JA SHORT client.52074265	; If above then jump to error handling part
	...
	5207424C  |> 53                        PUSH EBX	; Push string address onto stack as func argument
	5207424D  |. E8 BED92400               CALL client.522C1C10	; Call AllocPooledString() function
	52074252  |. 83C4 04                   ADD ESP,4	; Add 0x04 on ESP to free func argument space
	52074255  |. 8984B7 90090000           MOV DWORD PTR DS:[EDI+ESI*4+990],EAX	; Copy result value to name array entry (m_szName[slot]); EDI=this pointer, ESI=player slot, *4 due to c-string pointer, 0x990 offset to &m_szName
	...
	52074265  |> 56                        PUSH ESI	; Push ESI onto stack as func argument
	52074266  |. 68 9CD68452               PUSH client.5284D69C	;  Push string address onto stack as func arg: ASCII "UpdatePlayerName with bogus slot %d\n"
	5207426B  |. FF15 E4F37652             CALL DWORD PTR DS:[<&tier0.Error>]	; Call function tier0.Error()
	52074271  |. 83C4 08                   ADD ESP,8	; Add 0x08 on ESP to free func argument stack space
	52074274  |. 5F                        POP EDI	; Restore EDI
	52074275  |. 5E                        POP ESI	; Restore ESI
	52074276  |. 8BE5                      MOV ESP,EBP	; Copy EBP to ESP
	52074278  |. 5D                        POP EBP	; Restore EBP; ESP restored
	52074279  \. C2 0400                   RETN 4	; Restore return address and free 0x04 bytes func argument space

	The address of the C_PlayerResource object instance address can be retrieved via the CClientScoreBoardDialog::GetPlayerScoreInfo()
	method code:
	1A329500   55               PUSH EBP	; Backup old EBP onto stack
	1A329501   8BEC             MOV EBP,ESP	; Copy ESP to EBP, stack frame setup done
	1A329503   56               PUSH ESI	; Backup ESI onto stack
	1A329504   8B35 EC0BC31C    MOV ESI,DWORD PTR DS:[1CC30BEC]	; Copy DWORD at address to ESI. It's  the base address of the IGameResource object
	1A32950A   85F6             TEST ESI,ESI	; Perform TEST-operation with ESI
	1A32950C   0F84 CF000000    JE client.1A3295E1	; If no bit could be set (NULL-pointer) then jump to failure handler part
	...
	1A3295E1   32C0             XOR AL,AL	; Clear bits of AL (result = false)
	1A3295E3   5E               POP ESI	; Restore old ESI
	1A3295E4   5D               POP EBP	; Restore old EBP
	1A3295E5   C2 0800          RETN 8	; Restore return address and free additional argument space

	Disassembly of IVEngineClient::WorldToScreenMatrix() method:
	0F60FB50   A1 5804CE0F      MOV EAX,DWORD PTR DS:[FCE0458]
	0F60FB55   83F8 01          CMP EAX,1
	0F60FB58   7E 11            JLE SHORT engine.0F60FB6B
	0F60FB5A   69C8 10020000    IMUL ECX,EAX,210
	0F60FB60   A1 4C04CE0F      MOV EAX,DWORD PTR DS:[FCE044C]
	0F60FB65   83C0 BC          ADD EAX,-44
	0F60FB68   03C1             ADD EAX,ECX
	0F60FB6A   C3               RETN
	0F60FB6B   B8 0C04CE0F      MOV EAX,engine.0FCE040C
	0F60FB70   C3               RETN

	Disassembly of IVEngineClient::GetLocalPlayer() method:
	0CBAF4B0   A1 24020B0D      MOV EAX,DWORD PTR DS:[D0B0224]	; Copy object base address from pointer to EAX
	0CBAF4B5   8B80 60010000    MOV EAX,DWORD PTR DS:[EAX+160]	; Copy value from base + offset (local player number)
	0CBAF4BB   40               INC EAX	; Increment for valid slot number (due to player slots start at 1, not 0)
	0CBAF4BC   C3               RETN	; Restore return address
*/

//======================================================================
#define VCLIENTENTITYLIST_INTERFACE_VERSION	"VClientEntityList003" //Export name for this interface
#define MAX_INTERFACE_NAMELEN 20 //Enough characters to save a name
#define MAX_CVAR_NAMELEN 30 //Maximum amount of chars for internal cvar name
#define CVAR_NOT_FOUND 0xFFFFFFFF //Invalid CVar ident (not found)
#define MIN_PLAYER_SLOTS 1 //Lowest player index
#define MAX_PLAYER_SLOTS 64 //Highest player index
#define MAX_PLAYER_NAMELEN 32 //As defined in SDK (see MAX_PLAYER_NAME_LENGTH define)
#define MAX_CLIENTS 64
#define TEAM_CT 2
#define TEAM_T 3
//======================================================================

//======================================================================
enum cvar_patch_type_e { //CVar data value types
	CPT_UNKNOWN = -1,
	CPT_DWORD,
	CPT_FLOAT
};

enum cvar_module_type_e { //Module types
	CMT_UNKNOWN = -1,
	CMT_ENGINE,
	CMT_CLIENT
};
//======================================================================

//======================================================================
typedef unsigned int HENTITY;
typedef vec_t w2smatrix_t[4][4];
//======================================================================

//======================================================================
struct client_interface_export {
	LPVOID lpvFunction; //Pointer to interface function
	char* pszExportName; //Pointer to interface name
	client_interface_export* pNextExport; //Pointer to next entry
};

struct cvar_patch_s { //CVar information
	char szName[MAX_CVAR_NAMELEN]; //CVar string
	DWORD dwModuleBase; //Module base
	DWORD dwOffset; //Offset to cvar value
	cvar_patch_type_e cvartype; //CVar value type
	DWORD dwTypeSize; //CVar type size
};

struct entity_info_s {
	DWORD dwVFTable;
	unsigned char __ucPadding_1[0x60];
	int m_iIndex;
	unsigned long m_EntClientFlags;
	struct model_t* m_pModel;
	color32_s m_clrRender;
	unsigned char __ucPadding_2[11 * sizeof(unsigned int)];
	Vector m_vecAbsOrigin;
	Vector m_vecOrigin;
	unsigned char __ucPadding_3[12];
	QAngle m_vecViewangles;
	unsigned char __ucPadding_4[20];
	unsigned long m_iEFlags;
	char m_nWaterType;
	bool m_bDormant;
	unsigned char __ucPadding_5[2];
	int m_fEffects;
	int m_iTeamNum;
	unsigned char __ucPadding_6[8];
	int m_iHealth;
	char __ucPadding_7[68];
	EHANDLE m_hNetworkMoveParent, m_hOwnerEntity, m_hGroundEntity;
	char m_iName[MAX_PATH];
	short m_nModelIndex;
	unsigned char m_nRenderFX, m_nRenderMode, m_MoveType, m_MoveCollide, m_nWaterLevel;
	unsigned char m_lifeState;
};
//======================================================================

//======================================================================
class CGameSpecific : public CProcess, public COffsetMgr {
private:
	bool m_bReady; //Whether component is ready
	
	PROCESS_INFORMATION* m_pPI; //Process related information
	PROCESSENTRY32* m_pPE; //Process entry information

	CClientEntityList* m_pClientEntityList; //Pointer to IClientEntityList class instance
	HENTITY* m_pEntityList; //Pointer to IClientEntityList::m_EntityCacheInfo class member
	string_t* m_pNameList; //Pointer to name list of C_PlayerResource
	VMatrix* m_pW2SMatrix; //Pointer to world-2-screen matrix
	DWORD* m_pLocalPlayerAddr; //Pointer to player related object instance

	client_interface_export* FindExportInterfaceRegsEx(LPVOID lpvpfnCreateInterfaceOfClientDll);
	LPVOID FindClientExportEx(LPVOID lpvpfnCreateInterfaceOfClientDll, const std::string& lpszExportName);
	CClientEntityList* GetCClientEntityListPtrEx(LPVOID lpvpfnInterfaceFnctn);
	VMatrix* FindW2SMatrix(DWORD dwEngineDllBase);
public:
	CGameSpecific() : m_bReady(false), m_pPI(NULL), m_pPE(NULL), m_pEntityList(NULL), m_pNameList(NULL), m_pW2SMatrix(NULL) {}
	~CGameSpecific() { this->Clear(); }

	bool Initialize(const std::string& szProcName);
	bool Clear(void);

	bool AddOffset(const std::string& szName, DWORD dwOffset);
	void ClearOffsets(void);

	int GetMaxEntities(void);
	int GetMaxClients(void);
	HENTITY GetEntityByIndex(int iIndex);
	bool GetEntityInfoByHandle(const HENTITY hEntity, entity_info_s* pInfoOut);
	std::string GetPlayerName(const int iPlayerIndex);
	bool IsEntityPlaying(const entity_info_s& rEntityInfo);

	VMatrix* WorldToScreenMatrix(void);
	bool WorldToScreenMatrix(w2smatrix_t* pvMatrixOut);

	int GetLocalPlayerIndex(void);
};
//======================================================================

#endif
