#include "process.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: process.cpp: Process manager implementation
*/

//======================================================================
BOOL CProcess::GetProcessId(LPCSTR lpProcName)
{
	//Get process information of target process
	
	if (!lpProcName)
		return FALSE;

	//Create process list snapshot
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnap)
		return FALSE;
	
	this->m_pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &this->m_pe)) { //First process is system process
		while (Process32Next(hSnap, &this->m_pe)) { //Enumerate trough list entries
			if (strcmp(this->m_pe.szExeFile, lpProcName)==0) {//If target process is found we have it
				this->m_PI.dwProcessId = this->m_pe.th32ProcessID;
				
				CloseHandle(hSnap);

				return TRUE;
			}
		}
	}

	ZeroMemory(&this->m_pe, sizeof(this->m_pe)); //If failed zero bytes
	CloseHandle(hSnap);

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CProcess::OpenProcess(DWORD dwProcessId, DWORD dwDesiredRights)
{
	//Create a process stream
	
	if (!dwProcessId)
		return FALSE;

	this->m_PI.dwProcessId = dwProcessId;
	
	//Open process stream
	this->m_PI.hProcess = ::OpenProcess(dwDesiredRights, FALSE, dwProcessId);
	if (!this->m_PI.hProcess)
		return FALSE;

	return this->m_bPrInit = TRUE;
}
//======================================================================

//======================================================================
BOOL CProcess::ModuleAlreadyExists(LPCSTR lpModuleName)
{
	//Check if we have already that module in list

	if (!lpModuleName)
		return FALSE;

	//Search in list
	for (size_t i = 0; i < this->m_vProcModules.size(); i++) {
		if (strcmp(this->m_vProcModules[i].szName, lpModuleName)==0)
			return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CProcess::GetModuleBase(LPCSTR lpModuleName)
{
	//Get module information of target module in process context

	if ((!this->m_bPrInit) || (!lpModuleName))
		return FALSE;
	
	//Create module list snapshot
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->m_PI.dwProcessId);
	if (!hSnap)
		return FALSE;
	
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnap, &me)) { //First module is main module
		
		while (Module32Next(hSnap, &me)) { //Enumerate trough list entries
			if ((strcmp(me.szModule, lpModuleName)==0) && (!ModuleAlreadyExists(lpModuleName))) { //We have found our module
				CloseHandle(hSnap);

				//Add found module to list
				proc_module_s dll;
				strcpy_s(dll.szName, lpModuleName);
				memcpy(&dll.me, &me, sizeof(PROCESSENTRY32));
				this->m_vProcModules.push_back(dll);

				return TRUE;
			}
		}
	}

	CloseHandle(hSnap);
	ZeroMemory(&me, sizeof(me)); //We did not find it, so zero bytes

	return FALSE;
}
//======================================================================

//======================================================================
DWORD CProcess::GetModuleHandleByName(LPCSTR lpModuleName)
{
	//Get module handle by module name string

	if (!lpModuleName)
		return 0;

	//Search in list
	for (size_t i = 0; i < this->m_vProcModules.size(); i++) {
		if (strcmp(this->m_vProcModules[i].szName, lpModuleName)==0)
			return (DWORD)this->m_vProcModules[i].me.modBaseAddr;
	}

	return 0;
}
//======================================================================

//======================================================================
BOOL CProcess::ReadFromProcess(LPCVOID lpBaseAddr, LPVOID lpOut, DWORD dwMemSize)
{
	//Read data from a process 
	
	if ((!this->m_bPrInit) || (!lpOut) || (!dwMemSize))
		return FALSE;

	DWORD dwReadSize = 0;
	
	//Read the data into the specified buffer
	BOOL bRet = ReadProcessMemory(this->m_PI.hProcess, lpBaseAddr, lpOut, dwMemSize, &dwReadSize);

	return (bRet == TRUE) && (dwReadSize == dwMemSize);
}
//======================================================================

//======================================================================
BOOL CProcess::WriteToProcess(LPVOID lpBaseAddr, LPVOID lpIn, DWORD dwMemSize)
{
	//Read data from a process 
	
	if ((!this->m_bPrInit) || (!lpIn) || (!dwMemSize))
		return FALSE;

	DWORD dwWriteSize = 0;
	
	//Read the data into the specified buffer
	BOOL bRet = WriteProcessMemory(this->m_PI.hProcess, lpBaseAddr, lpIn, dwMemSize, &dwWriteSize);

	return (bRet == TRUE) && (dwWriteSize == dwMemSize);
}
//======================================================================

//======================================================================
BOOL CProcess::CloseProcess(VOID)
{
	//Finish the process stuff
	
	if (this->m_bPrInit) {
		//Close process handle
		CloseHandle(this->m_PI.hProcess);

		//Zero bytes
		Clear();
		
		this->m_bPrInit = FALSE;
		
		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
PROCESS_INFORMATION* CProcess::ProcessInformation(VOID)
{
	//Get process information

	return &this->m_PI;
}
//======================================================================

//======================================================================
PROCESSENTRY32* CProcess::ProcessEntry(VOID)
{
	//Get process entry data

	return &this->m_pe;
}
//======================================================================

//======================================================================
VOID CProcess::Clear(VOID)
{
	//Zero bytes of buffers
	
	this->m_bPrInit = FALSE;

	ZeroMemory(&this->m_PI, sizeof(this->m_PI));
	ZeroMemory(&this->m_pe, sizeof(this->m_pe));

	this->m_vProcModules.clear();
}
//======================================================================
