#include "vars.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: vars.cpp: Global variable declarations
*/

//======================================================================
#ifdef _BUILD_DLL
HMODULE g_hOwnDllHandle = NULL; //Handle to own DLL
#endif

global_objects_s g_Objects; //Global object table

features_s g_Features = { //Global features table

};

HANDLE g_hMutex = INVALID_HANDLE_VALUE; //Handle to mutex

HANDLE g_hCollectThread = NULL; //Handle to data collection thread

HANDLE g_hThreadStartEvent = NULL; //Handle to event which is used to inform the thread that it shall start collecting data
HANDLE g_hThreadEndEvent = NULL; //Handle to event which is used to inform the thread that it shall end

HHOOK g_hMouseHook = NULL; //Handle to mouse hook
HHOOK g_hKeyboardHook = NULL; //Handle to keyboard hook

POINT g_sCursorPos = {0, 0}; //Cursor position

std::string g_szBasePath; //Absolute path to program executable

HWND g_hTargetWindow = NULL; //Handle to target window
HWND g_hConsoleWindow = NULL; //Handle to console window

POINT g_TargetWindowRes = { //Shall hold target window resolution
	0, 0
};

team_color_s g_TeamColors[3] = { //Team color table
	{200, 200, 200, 150}, //Default
	{250, 0, 0, 150},
	{0, 0, 250, 150}
};

CzyVisualComponents::CForm* g_pMenuForm = NULL; //Pointer to menu component

CzyConfigMgr::CCVar::cvar_s* g_pcvNameESP = NULL; //CVar for name ESP
CzyConfigMgr::CCVar::cvar_s* g_pcvDistanceESP = NULL; //CVar for distance ESP
CzyConfigMgr::CCVar::cvar_s* g_pcvHealthESP = NULL; //CVar for health ESP
CzyConfigMgr::CCVar::cvar_s* g_pcvIgnoreTeammatesESP = NULL; //CVar for ESP color style
//======================================================================
