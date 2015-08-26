#ifndef _SYG_WINDOW_H
#define _SYG_WINDOW_H

#include "includes.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: window.h: Window manager interface
*/

//======================================================================
typedef void (*TpfnDrawEvent)(void);
//======================================================================

//======================================================================
class CWindow {
private:
	ATOM m_aClass;
	HWND m_hWindow;
	HWND m_hTarget;
	RECT m_rWndPosData;
	bool m_bShowToggle;
	bool m_bHideToggle;
	std::wstring m_szClassName;
	TpfnDrawEvent m_pDrawEvent;

	friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	CWindow() : m_aClass(NULL), m_hWindow(NULL), m_hTarget(NULL), m_bShowToggle(false), m_bHideToggle(false), m_pDrawEvent(NULL) { memset(&m_rWndPosData, 0x00, sizeof(m_rWndPosData)); }
	~CWindow() { this->Clear(); }

	HWND Initialize(HWND hTargetWindow, INT& riWidth, INT& riHeight, const std::wstring& szClassName, const TpfnDrawEvent pfnDrawEvent);
	bool Clear(void);

	void Process(void);

	bool TargetWindowExists(void);
	bool TargetWindowIsInForeground(void);

	int GetTopValue(void) { return this->m_rWndPosData.top; }
	int GetLeftValue(void) { return this->m_rWndPosData.left; }

	int GetResolutionX(void) { return this->m_rWndPosData.right - this->m_rWndPosData.left; }
	int GetResolutionY(void) { return this->m_rWndPosData.bottom - this->m_rWndPosData.top; }
};
//======================================================================

#endif
