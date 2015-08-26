#ifndef _PROCESS_H
#define _PROCESS_H

#include "includes.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: process.h: Process interface
*/

//======================================================================
#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next
#undef MODULEENTRY32
#undef Module32First
#undef Module32Next
//======================================================================

//======================================================================
struct proc_module_s {
	char szName[MAX_PATH];
	MODULEENTRY32 me;
};
//======================================================================

//======================================================================
class CProcess {
private:
	BOOL m_bPrInit;
	PROCESS_INFORMATION m_PI;
	PROCESSENTRY32 m_pe;

	std::vector<proc_module_s> m_vProcModules;

	BOOL ModuleAlreadyExists(LPCSTR lpModuleName);
public:
	CProcess() { }
	~CProcess() { this->Clear(); }

	BOOL GetProcessId(LPCSTR lpProcName);
	BOOL GetModuleBase(LPCSTR lpModuleName);
	DWORD GetModuleHandleByName(LPCSTR lpModuleName);

	BOOL OpenProcess(DWORD dwProcessId, DWORD dwDesiredRights);
	BOOL CloseProcess(VOID);
	
	BOOL ReadFromProcess(LPCVOID lpBaseAddr, LPVOID lpOut, DWORD dwMemSize);
	BOOL WriteToProcess(LPVOID lpBaseAddr, LPVOID lpIn, DWORD dwMemSize);

	PROCESS_INFORMATION* ProcessInformation(VOID);
	PROCESSENTRY32* ProcessEntry(VOID);
	MODULEENTRY32* ModuleEntry(VOID);
	THREADENTRY32* ThreadEntry(VOID);
	HWND MainWindow(VOID);
	
	VOID Clear(VOID);
};
//======================================================================

#endif
