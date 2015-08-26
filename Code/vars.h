#ifndef _VARS_H
#define _VARS_H

#include "includes.h"
#include "gamespec.h"
#include "keys.h"
#include "window.h"
#include "renderer.h"
#include "viscomps.h"
#include "utils.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: vars.h: Vars interface
*/

//======================================================================
#define PROGRAM_NAME "TigerSnail CS:GO External"
#define PROGRAM_VERSION "0.1"
#define PROGRAM_AUTHOR "sk0r / Czybik"
#define PROGRAM_CONTACT "Czybik_Stylez<0x40>gmx<0x2E>de"
#define PROGRAM_INFO PROGRAM_NAME " v" PROGRAM_VERSION " coded by " PROGRAM_AUTHOR " (" PROGRAM_CONTACT ")"

#define TARGET_PROCESS "csgo.exe"
#define TARGET_WINDOW "Counter-Strike: Global Offensive"
#define CONFIG_SCRIPT_FILE "config.cfg"

//#define _BUILD_EXE //If an application shall be build
#define _BUILD_DLL //If a dynamic link library shall be build
#if defined(_BUILD_EXE) && defined(_BUILD_DLL)
#error "Please specify either _BUILD_EXE or _BUILD_DLL"
#endif
#ifdef _BUILD_DLL
#define _BASE_DIR_TYPE 2 //Possible values: 1 = static path, 2 = from AppData, 3 = from DLL loading path
#define _STATIC_BASE_NAME "TigerSnail_CSGO_External"
#if _BASE_DIR_TYPE == 1
#define _STATIC_BASE_DIR "D:\\" _STATIC_BASE_NAME "\\"
#endif
#endif
//======================================================================

//======================================================================
struct global_objects_s {
	CGameSpecific game; //Game specific object instance
	CKeyManager keys; //Key manager object instance
	CWindow window; //Window manager object instance
	CRenderer renderer; //Renderer manager object instance
	CzyConfigMgr::CConfigInt cfg; //Config manager object instance
	class CTempPlayerData temppldata; //Temporary player data manager object instance
};

struct features_s {

};

struct team_color_s {
	byte r, g, b, a;
};
//======================================================================

//======================================================================
#ifdef _BUILD_DLL
extern HMODULE g_hOwnDllHandle;
#endif
extern global_objects_s g_Objects;
extern features_s g_Features;
extern HANDLE g_hMutex;
extern HANDLE g_hCollectThread;
extern HANDLE g_hThreadStartEvent;
extern HANDLE g_hThreadEndEvent;
extern HHOOK g_hMouseHook;
extern HHOOK g_hKeyboardHook;
extern POINT g_sCursorPos;
extern std::string g_szBasePath;
extern HWND g_hTargetWindow;
extern HWND g_hConsoleWindow;
extern POINT g_TargetWindowRes;
extern team_color_s g_TeamColors[3];
extern CzyVisualComponents::CForm* g_pMenuForm;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvNameESP;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvDistanceESP;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvHealthESP;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvIgnoreTeammatesESP;
//======================================================================

#endif
