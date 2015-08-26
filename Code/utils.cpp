#include "utils.h"
#include "vars.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: utils.cpp: Implementation of various utility functions
*/

//======================================================================
VOID PrintConsole(WORD wAttr, const char* lpszFmt, ...)
{
	//Write a text with the specified console text attributes to the standard output

	if (!lpszFmt)
		return;

	char szMsg[MAX_PATH];
	
	//Format message
	va_list List;
	va_start(List, lpszFmt);
	vsprintf(szMsg, lpszFmt, List);
	va_end(List);

	//Get handle to standard output and standard console text attributes at first call
	//Regarding the text attributes we use the current one. Please don't change it
	//outside of this function to ensure having the standard values.
	static bool bOnce = false;
	static HANDLE hStdOut = 0;
	static CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
	if (!bOnce) {
		bOnce = true;

		//Get handle to standard output
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!hStdOut)
			FatalError("Error: GetStdHandle failed: %d", GetLastError());

		//Get standard console screen buffer info
		if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
			FatalError("Error: GetConsoleScreenBufferInfo failed: %d", GetLastError());
	}

	//Correct attribute argument if equal to zero
	if (wAttr == CONSOLE_ATTRIBUTE_DEFAULT)
		wAttr = csbi.wAttributes;

	//Set console text attributes
	if (!SetConsoleTextAttribute(hStdOut, wAttr))
		FatalError("Error: SetConsoleTextAttribute failed: %d", GetLastError());

	//Send text string to standard output
	DWORD dwCharsWritten;
	if (!WriteConsoleA(hStdOut, szMsg, strlen(szMsg), &dwCharsWritten, NULL))
		FatalError("Error: WriteConsole() failed: %d", GetLastError());

	//Restore old console text attributes
	if (!SetConsoleTextAttribute(hStdOut, csbi.wAttributes))
		FatalError("Error: SetConsoleTextAttribute failed: %d", GetLastError());
}
//======================================================================

//======================================================================
void FatalError(const char* lpszFmt, ...)
{
	//Handle fatal errors here. This function will terminate the own process
	//Note: A message is urgently required!

	if (!lpszFmt)
		return;

	char szMsg[MAX_PATH];
 
	//Format message
	va_list List;
	va_start(List, lpszFmt);
	vsprintf(szMsg, lpszFmt, List);
	va_end(List);

	//Show message box
	MessageBoxA(0, szMsg, PROGRAM_NAME " :: Fatal Error", MB_ICONERROR);

	//Exit process with failure
	exit(EXIT_FAILURE);
}
//======================================================================

//======================================================================
bool GetAbsoluteBasePath(std::string& szOut)
{
	//Get absolute base path. "Base" path means the
	//path of which stuff shall get handled (configs etc.)

#if (defined(_BUILD_EXE)) || ((defined(_BUILD_DLL)) && (_BASE_DIR_TYPE != 1))
	char szFileName[MAX_PATH];
#endif

	//Retrieve full name of own process main module
#if (defined(_BUILD_EXE)) || ((defined(_BUILD_DLL)) && (_BASE_DIR_TYPE == 3))
#if defined(_BUILD_EXE)
	if (!GetModuleFileNameA(0, szFileName, sizeof(szFileName)))
#elif defined(_BUILD_DLL)
	if (!GetModuleFileNameA(g_hOwnDllHandle, szFileName, sizeof(szFileName)))
#endif
		return false;

	//Delete file name chars
	for (size_t i = strlen(szFileName); i > 3; i--) {
		if (szFileName[i] == '\\')
			break;

		szFileName[i] = 0;
	}

	szOut = szFileName; //Save string
#endif

#if defined(_BUILD_DLL)
#if _BASE_DIR_TYPE == 1
	szOut = _STATIC_BASE_DIR;
#elif _BASE_DIR_TYPE == 2
	if (!SHGetSpecialFolderPathA(NULL, szFileName, CSIDL_APPDATA, FALSE))
		return false;

	szOut = std::string(szFileName) + "\\" _STATIC_BASE_NAME "\\"; //Save string
#endif
#endif

	return true;
}
//======================================================================

//======================================================================
DWORD HexToSignedInt(const std::string& szHexStr)
{
	//Convert hex string to signed integer

	if (!szHexStr.length())
		return 0;

	DWORD dwResult = 0;
	std::stringstream ss;
	ss << std::hex << szHexStr;
	ss >> dwResult;

	return dwResult;
}
//======================================================================

//======================================================================
bool IsValidWindow(HWND hWindow)
{
	//Indicate whether the window specified by the handle is still valid
	//Valid means for handling keys, so it's valid if the window still exists
	//and if it is currently in foreground

	return (IsWindow(hWindow)) && (GetForegroundWindow() == hWindow);
}
//======================================================================

//======================================================================
float GetDistance(const Vector& vSource, const Vector& vDest)
{
	//Calculate distance between two vectors

	#define FSQR(v) (v * v)

	Vector vDist = vDest - vSource;

	return sqrt(FSQR(vDist.x) + FSQR(vDist.y) + FSQR(vDist.z));
}
//======================================================================

//======================================================================
void DrawCursor(void)
{
	//Draw cursor for menu

	#define MENU_CURSOR_LENGTH 10
	#define MENU_CURSOR_SIZE 2

	int iCurCursorX = g_sCursorPos.x;
	int iCurCursorY = g_sCursorPos.y;

	g_Objects.renderer.DrawFilledBox(iCurCursorX - MENU_CURSOR_LENGTH / 2, iCurCursorY, MENU_CURSOR_LENGTH + MENU_CURSOR_SIZE, MENU_CURSOR_SIZE, 255, 0, 0, 255);
	g_Objects.renderer.DrawFilledBox(iCurCursorX, iCurCursorY - MENU_CURSOR_LENGTH / 2, MENU_CURSOR_SIZE, MENU_CURSOR_LENGTH + MENU_CURSOR_SIZE, 255, 0, 0, 255);
}
//======================================================================
