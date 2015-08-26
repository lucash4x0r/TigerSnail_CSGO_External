#include "window.h"
#include <dwmapi.h>

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: window.cpp: Window manager implementations
*/

//======================================================================
CWindow *pWindowThisInstance = NULL;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Window message processing callback function
	
	if (uMsg == WM_PAINT) { //Window shall be painted
		pWindowThisInstance->m_pDrawEvent(); //Call event function

		return 0;
	} else if (uMsg == WM_DESTROY) { //Window shall get destroyed
		PostQuitMessage(EXIT_SUCCESS); //Request thread termination
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam); //Let Windows perform default message handling for messages this routine does not handle
}
//======================================================================

//======================================================================
HWND CWindow::Initialize(HWND hTargetWindow, INT& riWidth, INT& riHeight, const std::wstring& szClassName, const TpfnDrawEvent pfnDrawEvent)
{
	//Create overlay window for target window

	if ((!hTargetWindow) || (!pfnDrawEvent))
		return NULL;

	//Get target window information
	if (!GetWindowRect(hTargetWindow, &this->m_rWndPosData))
		return NULL;

	this->m_szClassName = szClassName;

	//Setup data
	WNDCLASSEX wndClassData;
	memset(&wndClassData, 0x00, sizeof(wndClassData));
	wndClassData.cbSize = sizeof(WNDCLASSEX);
	wndClassData.style = CS_VREDRAW | CS_HREDRAW;
	wndClassData.lpfnWndProc = &WindowProc;
	wndClassData.hInstance = (HINSTANCE)GetCurrentProcess();
	wndClassData.lpszClassName = this->m_szClassName.c_str();
	
	//Register class
	this->m_aClass = RegisterClassEx(&wndClassData);
	if (!this->m_aClass)
		return NULL;

	//Create window
	this->m_hWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, this->m_szClassName.c_str(), this->m_szClassName.c_str(), WS_POPUP, this->m_rWndPosData.left, this->m_rWndPosData.top, this->m_rWndPosData.right - this->m_rWndPosData.left, this->m_rWndPosData.bottom - this->m_rWndPosData.top, NULL, NULL, 0, 0);
	if (!this->m_hWindow) {
		UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());
		this->m_aClass = NULL;
		return NULL;
	}

	//Set transparency color key of window
	if (!SetLayeredWindowAttributes(this->m_hWindow, RGB(0, 0, 0), 0, LWA_COLORKEY)) {
		DestroyWindow(this->m_hWindow);
		UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());
		this->m_aClass = NULL;
		return NULL;
	}

	//Extend window frame into client area
	MARGINS mMargin = {0, 0, this->m_rWndPosData.right - this->m_rWndPosData.left, this->m_rWndPosData.bottom - this->m_rWndPosData.top};
    if (FAILED(DwmExtendFrameIntoClientArea(this->m_hWindow, &mMargin))) {
		DestroyWindow(this->m_hWindow);
		UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());
		this->m_aClass = NULL;
		return NULL;
	}

	//Set resolution
	riWidth = this->m_rWndPosData.right - this->m_rWndPosData.left;
	riHeight = this->m_rWndPosData.bottom - this->m_rWndPosData.top;

	//Save target window handle
	this->m_hTarget = hTargetWindow;

	//Save event function pointer
	this->m_pDrawEvent = pfnDrawEvent;

	//Set instance pointer
	pWindowThisInstance = this;

	return this->m_hWindow;
}
//======================================================================

//======================================================================
bool CWindow::Clear(void)
{
	//Clear data

	if (!this->m_aClass)
		return false;

	//Destroy window
	if (!DestroyWindow(this->m_hWindow))
		return false;

	//Unregister class
	if (!UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess()))
		return false;

	return true;
}
//======================================================================

//======================================================================
void CWindow::Process(void)
{
	//Handle window messages

	MSG mMsg;
	
	if (PeekMessage(&mMsg, this->m_hWindow, 0, 0, PM_REMOVE)) { //Check for incoming messages from the message queue for this specific window
		DispatchMessage(&mMsg); //Send message to window procedure
		TranslateMessage(&mMsg); //Translate key messages to character messages for the calling threads message queue
	}

	RECT rData;

	//Get target window data
	if (GetWindowRect(this->m_hTarget, &rData)) {
		//Check if window position and resolution has changed and if so, update our window

		if (memcmp(&rData, &this->m_rWndPosData, sizeof(RECT))) { //Check if memory field data values are mismatching
			//Correct our window
			if (MoveWindow(this->m_hWindow, rData.left, rData.top, rData.right, rData.bottom, TRUE))
				memcpy(&this->m_rWndPosData, &rData, sizeof(RECT)); //Update comparision data
		}
	}

	//Show our window only if the target window is the foreground window
	if (GetForegroundWindow() == this->m_hTarget) {
		if (!this->m_bShowToggle) {
			ShowWindow(this->m_hWindow, SW_SHOW); //Show our window at first time
			this->m_bShowToggle = true;
		}

		if (this->m_bHideToggle)
			this->m_bHideToggle = false; //Reset
	} else {
		if (this->m_bShowToggle)
			this->m_bShowToggle = false; //Reset

		if (!this->m_bHideToggle) {
			ShowWindow(this->m_hWindow, SW_HIDE); //Hide our window at first time
			this->m_bHideToggle = true;
		}
	}
}
//======================================================================

//======================================================================
bool CWindow::TargetWindowExists(void)
{
	//Check if target window is still valid

	return IsWindow(this->m_hTarget) == TRUE;
}
//======================================================================

//======================================================================
bool CWindow::TargetWindowIsInForeground(void)
{
	//Check if target window is still valid
	
	return GetForegroundWindow() == this->m_hTarget;
}
//======================================================================
