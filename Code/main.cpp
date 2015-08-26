#include "includes.h"
#include "vars.h"
#include "utils.h"
#include "wordtoscreen.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: main.cpp: Main implementaions
*/

//======================================================================
bool bMainReady = false; //Whether Main_Think may do its job or not
bool bMainLoopRunning = false; //Flag used to escape from main loop
//======================================================================

//======================================================================
void Main_Startup(void);
inline bool Main_Ready(void);
DWORD WINAPI CollectEntityData(LPVOID lpvArguments);
void Main_Process(void);
void Main_Shutdown(void);
//======================================================================

//======================================================================
void Cmd_Echo(void)
{
	//Output message

	std::string szMsg = g_Objects.cfg.ExpressionItemValue(1);

	PrintConsole(FOREGROUND_PINK, "%s\n", szMsg.c_str());
}
//======================================================================

//======================================================================
void Cmd_ClearOffsets(void)
{
	//Clear offsets

	g_Objects.game.ClearOffsets();
}
//======================================================================

//======================================================================
void Cmd_AddOffset(void)
{
	//Add new offset

	const size_t uiArgCount = g_Objects.cfg.ExpressionItemCount() - 1;
	if (uiArgCount != 2) {
		FatalError("[Cmd_AddOffset] Need two arguments, %d given", uiArgCount);
		return;
	}

	std::string szName = g_Objects.cfg.ExpressionItemValue(1);
	std::string szValue = g_Objects.cfg.ExpressionItemValue(2);

	if (!g_Objects.game.AddOffset(szName, HexToSignedInt(szValue))) {
		FatalError("CGameSpecific::AddOffset() failed for %s %s", szName.c_str(), szValue.c_str());
	} else {
		PrintConsole(FOREGROUND_RED | FOREGROUND_GREEN, "[Offsets] Added offset %s (%s)\n", szName.c_str(), szValue.c_str());
	}
}
//======================================================================

//======================================================================
void Cmd_ClearKeys(void)
{
	//Clear keys

	g_Objects.keys.Clear();
}
//======================================================================

//======================================================================
void Cmd_AddKey(void)
{
	//Add new key info

	const size_t uiArgCount = g_Objects.cfg.ExpressionItemCount() - 1;
	if (uiArgCount != 2) {
		FatalError("[Cmd_AddKey] Need two arguments, %d given", uiArgCount);
		return;
	}

	std::string szName = g_Objects.cfg.ExpressionItemValue(1);
	std::string szValue = g_Objects.cfg.ExpressionItemValue(2);

	if (!g_Objects.keys.AddKey(szName, atoi(szValue.c_str()))) {
		FatalError("CKeyManager::AddKey() failed for %s %s", szName.c_str(), szValue.c_str());
	} else {
		PrintConsole(FOREGROUND_RED | FOREGROUND_GREEN, "[Keys] Added key info %s (%s)\n", szName.c_str(), szValue.c_str());
	}
}
//======================================================================

//======================================================================
//D3D drawing interface functions

BOOL WINAPI D3DI_PrintText(LPCSTR lpszMessage)
{
	PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "%s\n", lpszMessage);

	return TRUE;
}

BOOL WINAPI D3DI_DrawText(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_Objects.renderer.DrawString(lpszText, lpszFont, ucFontSizeW, ucFontSizeH, x, y, r, g, b, a);
}

BOOL WINAPI D3DI_DrawBox(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_Objects.renderer.DrawBox(x, y, w, h, iThickness, r, g, b, a);
}

BOOL WINAPI D3DI_DrawFilledBox(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_Objects.renderer.DrawFilledBox(x, y, w, h, r, g, b, a);
}

BOOL WINAPI D3DI_DrawLine(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_Objects.renderer.DrawLine(x1, y1, x2, y2, r, g, b, a);
}

size_t WINAPI D3DI_LoadImage(LPCSTR lpszFileName)
{
	return g_Objects.renderer.LoadImage(lpszFileName);
}

BOOL WINAPI D3DI_DrawImage(const size_t uiImageId, INT x, INT y)
{
	return g_Objects.renderer.DrawImage(uiImageId, x, y);
}
//======================================================================

//======================================================================
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	//Console handler function

	if (dwCtrlType == CTRL_CLOSE_EVENT) { //Console shall be closed
		bMainLoopRunning = false;

		Main_Shutdown();

		return TRUE; //Handled
	}

	return FALSE; //Not handled
}
//======================================================================

//======================================================================
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//Mouse hook procedure function

	if (g_pMenuForm->IsVisible()) { //Check if menu is currently visible
		if (nCode == HC_ACTION) { //Check if mouse information is available
			MSLLHOOKSTRUCT* pMouseHookData = (MSLLHOOKSTRUCT*)lParam; //Set pointer to mouse data struct

			if (wParam == WM_MOUSEMOVE) {
				//Handle mouse movement event

				//Save current position
				g_sCursorPos = pMouseHookData->pt;
				
				//Update new coordinates to subsystem
				if (!SetCursorPos(g_sCursorPos.x, g_sCursorPos.y))
					PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "Error: SetCursorPos() failed: %d\n", GetLastError());

				//Inform form
				g_pMenuForm->MouseEvent(g_sCursorPos.x, g_sCursorPos.y, -1, false);
			} else if (wParam == WM_LBUTTONUP) {
				//Handle mouse left button release event

				//Inform form
				g_pMenuForm->MouseEvent(g_sCursorPos.x, g_sCursorPos.y, VK_LBUTTON, false);
			} else if (wParam == WM_LBUTTONDOWN) {
				//Handle mouse left button push event

				//Inform form
				g_pMenuForm->MouseEvent(g_sCursorPos.x, g_sCursorPos.y, VK_LBUTTON, true);
			} else if (wParam == WM_RBUTTONUP) {
				//Handle mouse right button release event

				//Inform form
				g_pMenuForm->MouseEvent(g_sCursorPos.x, g_sCursorPos.y, VK_RBUTTON, false);
			} else if (wParam == WM_RBUTTONDOWN) {
				//Handle mouse right button push event

				//Inform form
				g_pMenuForm->MouseEvent(g_sCursorPos.x, g_sCursorPos.y, VK_RBUTTON, true);
			}

			return 1L; //Block input to other handlers
		}
	}

	//Proceed with hook handler chain
	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}
//======================================================================

//======================================================================
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//Keyboard hook procedure function

	if (g_pMenuForm->IsVisible()) { //Check if menu is currently visible
		if (nCode == HC_ACTION) { //Check if keyboard information is available
			KBDLLHOOKSTRUCT* pKeyboardHookData = (KBDLLHOOKSTRUCT*)lParam; //Set pointer to keyboard data struct

			if ((pKeyboardHookData->vkCode != VK_ESCAPE) && (pKeyboardHookData->vkCode != VK_MENU) && (pKeyboardHookData->vkCode != VK_F4) && (!g_Objects.keys.KeyExists(pKeyboardHookData->vkCode, NULL))) { //Specific keys may not be blocked
				//Determine if key is held down or is releaed
				const bool bIsKeyDown = (wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN);

				//Inform form
				g_pMenuForm->KeyEvent(pKeyboardHookData->vkCode, bIsKeyDown);

				return 1L; //Block input to other handlers
			}
		}
	}

	//Proceed with hook handler chain
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}
//======================================================================

//======================================================================
bool RegisterMenu(void)
{
	//Register menu

	static CzyVisualComponents::windowinfo_s sWindowInfo = { //Window info
		200, 200, //x y
		240, 145, //w h
		{
			//Colors
			{0, 0, 0, 255}, //Borders
			{255, 210, 225, 255}, //Closebox
			{135, 206, 250, 250}, //Headfill
			{240, 240, 240, 250}, //Bodyfill
			{50, 50, 50, 200},	//Cube
			{0, 0, 0, 250},	// Title
		},
		2, //Border size
		7, 15, //Font size (w h)
		0, //Font char dist
		5, //Font line dist
		5, //Box dist
		10, 10, //Cube size (w h)
		"Verdana" //Default font face
	};

	static CzyVisualComponents::drawinginterface_s sDrawingInterface = { //Drawing interface table
		D3DI_PrintText,
		D3DI_DrawText,
		D3DI_DrawBox,
		D3DI_DrawFilledBox,
		D3DI_DrawLine,
		D3DI_LoadImage,
		D3DI_DrawImage
	};

	static CzyVisualComponents::color32_s sTextColor = {
		0, 0, 0, 150
	};

	static CzyVisualComponents::color32_s sBoxColor = {
		255, 255, 255, 255
	};

	static CzyVisualComponents::color32_s sHoverColor = {
		255, 255, 225, 255
	};

	//Create the form
	g_pMenuForm = CzyVisualComponents::CreateForm("frmMenu", &sWindowInfo, &sDrawingInterface);
	if (!g_pMenuForm)
		return false;

	//Set location data
	g_pMenuForm->SetLocation(sWindowInfo.x, sWindowInfo.y);

	//Set menu title text
	g_pMenuForm->SetText(PROGRAM_NAME);

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMenuForm, "cbNameESP", 5, 40, &sTextColor, &sBoxColor, &sHoverColor, "Name ESP", "esp_name", g_pcvNameESP->bValue))
		return false;

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMenuForm, "cbDistanceESP", 5, 65, &sTextColor, &sBoxColor, &sHoverColor, "Distance ESP", "esp_distance", g_pcvDistanceESP->bValue))
		return false;

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMenuForm, "cbHealthESP", 5, 90, &sTextColor, &sBoxColor, &sHoverColor, "Health ESP", "esp_health", g_pcvHealthESP->bValue))
		return false;

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMenuForm, "cbIgnoreTeammatesESP", 5, 115, &sTextColor, &sBoxColor, &sHoverColor, "Ignore Teammates", "esp_ignoreteammates", g_pcvIgnoreTeammatesESP->bValue))
		return false;

	return true;
}
//======================================================================

//======================================================================
void Main_Startup(void)
{
	//Initialize hack

	if (bMainReady) //If already initialized
		return;

	//Add console control handle
	if (!SetConsoleCtrlHandler(&HandlerRoutine, TRUE))
		FatalError("Error: SetConsoleCtrlHandler failed: %d", GetLastError());

	//Change console title
	if (!SetConsoleTitleA(PROGRAM_NAME))
		FatalError("Error: SetConsoleTitleA failed: %d", GetLastError());

	//Print information message
	PrintConsole(FOREGROUND_RED | FOREGROUND_GREEN, PROGRAM_INFO "\n\n");

	//Create mutex
	g_hMutex = CreateMutexA(0, FALSE, PROGRAM_NAME);
	if ((g_hMutex == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		FatalError("Error: Hack is already running or mutex is already in use");
	}

	//Get absolute base path
	if (!GetAbsoluteBasePath(g_szBasePath))
		FatalError("Error: GetAbsoluteBasePath() failed: %d", GetLastError());

	PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "Using base path: \"%s\"\n\n", g_szBasePath);

	//Find CS:GO main window
	g_hTargetWindow = FindWindow(NULL, TEXT(TARGET_WINDOW));
	if (!g_hTargetWindow)
		FatalError("Error: FindWindow failed: %d\nPlease be sure  that %s is actually running", GetLastError(), TARGET_WINDOW);

	//Find console window
	g_hConsoleWindow = FindWindow(NULL, TEXT(PROGRAM_NAME));
	if (!g_hConsoleWindow)
		FatalError("Error: FindWindow failed: %d\nPlease be sure  that %s is actually running", GetLastError(), PROGRAM_NAME);

	//Set script directory
	g_Objects.cfg.SetScriptDirectory(g_szBasePath);

	//Register echo command, offset and key handlers
	g_Objects.cfg.CCommand::Add("echo", &Cmd_Echo);
	g_Objects.cfg.CCommand::Add("offsets.clear", &Cmd_ClearOffsets);
	g_Objects.cfg.CCommand::Add("offsets.add", &Cmd_AddOffset);
	g_Objects.cfg.CCommand::Add("keys.clear", &Cmd_ClearKeys);
	g_Objects.cfg.CCommand::Add("keys.add", &Cmd_AddKey);

	//Add CVars
	#define REG_CVAR(r, n, t, v) r = g_Objects.cfg.CCVar::Add(n, t, v); if (!r) FatalError("Error: Failed to add CVar %s", n);
	REG_CVAR(g_pcvNameESP, "esp_name", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");
	REG_CVAR(g_pcvDistanceESP, "esp_distance", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");
	REG_CVAR(g_pcvHealthESP, "esp_health", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");
	REG_CVAR(g_pcvIgnoreTeammatesESP, "esp_ignoreteammates", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");

	//Execute config file
	if (!g_Objects.cfg.Execute(CONFIG_SCRIPT_FILE))
		FatalError("Error: CCE_ExecScript failed for \"%s\"", CONFIG_SCRIPT_FILE);
	else
		PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "Executed script file \"%s%s\"\n\n", g_szBasePath.c_str(), CONFIG_SCRIPT_FILE);

	//Initialize game object
	if (!g_Objects.game.Initialize(TARGET_PROCESS)) {
		g_Objects.game.Clear();

		FatalError("Error: CGameSpecific::Initialize failed");
	}

	//Initialize GUI related stuff

	//Create window
	int x, y;
	HWND hDrawingWindow = g_Objects.window.Initialize(g_hTargetWindow, x, y, TEXT(PROGRAM_NAME), &Main_Process);
	if (!hDrawingWindow) {
		FatalError("Error: CWindow::Initialize() failed: %d", GetLastError());
	}

	//Save coordinates
	g_TargetWindowRes.x = x; g_TargetWindowRes.y = y;

	//Initialize D3D renderer
	if (!g_Objects.renderer.Initialize(hDrawingWindow, TRUE, x, y, 0, 0, 0, 0, std::string(g_szBasePath + "\\images").c_str())) {
		FatalError("Error: CRenderer::Initialize() failed: %d", GetLastError());
	}

	//Set config interface
	CzyVisualComponents::SetConfigInt(&g_Objects.cfg);

	//Register menu
	if (!RegisterMenu()) {
		FatalError("Error: RegisterMenu() failed: %d", GetLastError());
	}

	//Create start signal event
	g_hThreadStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!g_hThreadStartEvent)
		FatalError("Error: CreateEvent() failed: %d", GetLastError());

	//Create end signal event
	g_hThreadEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!g_hThreadEndEvent)
		FatalError("Error: CreateEvent() failed: %d", GetLastError());

	//Set event to be non-signaled
	if (!ResetEvent(g_hThreadStartEvent))
		FatalError("Error: ResetEvent() failed: %d", GetLastError());

	//Set event to be non-signaled
	if (!ResetEvent(g_hThreadEndEvent))
		FatalError("Error: ResetEvent() failed: %d", GetLastError());

	//Create data collection thread
	g_hCollectThread = CreateThread(NULL, 0, &CollectEntityData, NULL, 0, NULL);
	if (!g_hCollectThread)
		FatalError("Error: CreateThread() failed for \'CollectEntityData\': %d", GetLastError());

	//Hide menu at the beginning
	g_pMenuForm->Hide();

	//Set list size according to possible slots
	g_Objects.temppldata.Setup(g_Objects.game.GetMaxClients() + 1);

	bMainReady = true;
}
//======================================================================

//======================================================================
inline bool Main_Ready(void)
{
	//Return value of the ready flag

	return bMainReady;
}
//======================================================================

//======================================================================
DWORD WINAPI CollectEntityData(LPVOID lpvArguments)
{
	//Thread where to collect entity data

	//Wait until signal is given that states that the thread may start to collect data
	DWORD dwResult = WaitForSingleObject(g_hThreadStartEvent, INFINITE);
	if (dwResult != WAIT_OBJECT_0) {
		FatalError("Error: WaitForSingleObject() didn't return the expected value: %d %d", dwResult, GetLastError());
	}

	static int iLocalPlayerIndex = 0;
	static entity_info_s sLocalPlayerData;

	while (true) { //Continue permanently
		//Check if the thread end signal is given and then break out if that's the case
		if (WaitForSingleObject(g_hThreadEndEvent, 1) == WAIT_OBJECT_0)
			break;

		int iLocalPlayerIndex = g_Objects.game.GetLocalPlayerIndex(); //Get index number of local player

		for (int i = 1; i < g_Objects.game.GetMaxClients(); i++) { //Loop through possible player slots
			HENTITY hEntity = g_Objects.game.GetEntityByIndex(i); //Get handle to current entity of given slot
			if (hEntity) {
				entity_info_s sEntityInfo;

				if (g_Objects.game.GetEntityInfoByHandle(hEntity, &sEntityInfo)) { //Get C_BaseEntity data for this current entity
					if (g_Objects.game.IsEntityPlaying(sEntityInfo)) { //Check if player is playing actively
						std::string szPlayerName = g_Objects.game.GetPlayerName(sEntityInfo.m_iIndex); //Get name of this player

						//Check if this is the local player
						bool bIsLocalPlayer = i == iLocalPlayerIndex;

						//Handle stuff for local player
						if (bIsLocalPlayer) {
							sLocalPlayerData = sEntityInfo;
						}

						float fDistance = -1.0f;

						if ((!bIsLocalPlayer) && (iLocalPlayerIndex)) {
							//Handle stuff if this is not the local player and the local player is known

							//Calculate distance
							fDistance = GetDistance(sLocalPlayerData.m_vecAbsOrigin, sEntityInfo.m_vecAbsOrigin);
						}

						//Update slot data
						g_Objects.temppldata.UpdateSlot(i, szPlayerName, &sEntityInfo, fDistance, bIsLocalPlayer);
						
						Vector vScreenPos;
						w2smatrix_t vMatrix;

						//Get current world-2-screen matrix data
						if (!g_Objects.game.WorldToScreenMatrix(&vMatrix))
							continue;
					
						bool bResult = WorldToScreen(vMatrix, sEntityInfo.m_vecAbsOrigin, vScreenPos); //Calculate world position to screen position
						
						//Update slots screenpos data
						g_Objects.temppldata.UpdateSlot(i, vScreenPos, bResult);
					}
				}
			}
		}

		Sleep(1);
	}

	return EXIT_SUCCESS;
}
//======================================================================

//======================================================================
bool SetMenuInputActivation(const bool bStatus)
{
	//Enable or disable menu input handling according to gien status

	if (bStatus) {
		//Register low-level mouse hook
		#if defined(_BUILD_EXE)
		g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseHookProc, NULL, 0);
		#elif defined(_BUILD_DLL)
		g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MouseHookProc, g_hOwnDllHandle, 0);
		#endif
		if (!g_hMouseHook)
			return false;

		//Register low-level keyboard hook
		#if defined(_BUILD_EXE)
		g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardHookProc, NULL, 0);
		#elif defined(_BUILD_DLL)
		g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardHookProc, g_hOwnDllHandle, 0);
		#endif
		if (!g_hKeyboardHook)
			return false;
	} else {
		//Unregister low-level mouse hook
		if (!UnhookWindowsHookEx(g_hMouseHook))
			return false;

		//Unregister low-level keyboard hook
		if (!UnhookWindowsHookEx(g_hKeyboardHook))
			return false;

		//Clear values
		g_hMouseHook = g_hKeyboardHook = NULL;
	}

	return true;
}
//======================================================================

//======================================================================
void Main_Process(void)
{
	//Main processing function

	if (!bMainReady)
		return;

	//Check if the foreground window is not the target window and if so break out
	if (g_hTargetWindow != GetForegroundWindow())
		return;

	//Get into drawing state
	if (!g_Objects.renderer.DrawBegin()) {
		FatalError("CRenderer::DrawBegin() failed: %d", GetLastError());
	}

	static int iLocalPlayerTeam = TEAM_SPECTATOR;

	//Handle temporary entity data slots
	if ((g_pcvNameESP->bValue) || (g_pcvDistanceESP->bValue) || (g_pcvHealthESP->bValue)) { //At least one enhancement must be activated
		for (size_t i = 1; i < g_Objects.temppldata.Size(); i++) { //Loop through slots
			CTempPlayerData::temp_player_data_s* pTempPlayerData = g_Objects.temppldata.FromSlot(i); //Get current slot data pointer
			if ((pTempPlayerData) && (pTempPlayerData->bHasUpdatedOnce)) { //Validate values
				//Update localplayer team value if this is the local player
				if (pTempPlayerData->bIsLocalPlayer)
					iLocalPlayerTeam = pTempPlayerData->sEntInfo.m_iTeamNum;

				if ((!pTempPlayerData->bIsLocalPlayer) && (pTempPlayerData->bScreenPosValid)) {
				
					//Handle if teammates drawing shall be ignored
					if (g_pcvIgnoreTeammatesESP->bValue) {
						if (iLocalPlayerTeam == pTempPlayerData->sEntInfo.m_iTeamNum)
							continue;
					}

					#define LINE_OFFSET_ADD 14

					byte ucLineOffset = 0;
				
					color32_s clrTeamColor = {200, 200, 200, 255};
					if (pTempPlayerData->eTeam == CTempPlayerData::team_blue) { //Blue
						clrTeamColor.r = 121;
						clrTeamColor.g = 205;
						clrTeamColor.b = 205;
					} else if (pTempPlayerData->eTeam == CTempPlayerData::team_red) { //Red
						clrTeamColor.r = 210;
						clrTeamColor.g = 105;
						clrTeamColor.b = 30;
					}

					//Handle name ESP
					if (g_pcvNameESP->bValue) {
						g_Objects.renderer.DrawString(pTempPlayerData->szName.c_str(), "Verdana", 7, 15, (int)pTempPlayerData->vScreenPos.x, (int)pTempPlayerData->vScreenPos.y, clrTeamColor.r, clrTeamColor.g, clrTeamColor.b, clrTeamColor.a);
				
						ucLineOffset += LINE_OFFSET_ADD;
					}

					//Handle distance ESP
					if (g_pcvDistanceESP->bValue) {
						if (pTempPlayerData->fDistanceFromLocalPlayer != -1.0f) {
							std::string szDistance = std::to_string((_ULonglong)(int)pTempPlayerData->fDistanceFromLocalPlayer / 10);

							g_Objects.renderer.DrawString(szDistance.c_str(), "Verdana", 7, 15, (int)pTempPlayerData->vScreenPos.x, (int)pTempPlayerData->vScreenPos.y + ucLineOffset, (!ucLineOffset) ? clrTeamColor.r : 200, (!ucLineOffset) ? clrTeamColor.g : 200, (!ucLineOffset) ? clrTeamColor.b : 200, clrTeamColor.a);

							ucLineOffset += LINE_OFFSET_ADD;
						}
					}

					//Handle health ESP
					if (g_pcvHealthESP->bValue) {
						color32_s sHealthColor = {0, 255, 0, 255};
	
						if (pTempPlayerData->sEntInfo.m_iHealth <= 35) {
							sHealthColor.r = 255;
							sHealthColor.g = 0;
							sHealthColor.b = 0;
						} else if (pTempPlayerData->sEntInfo.m_iHealth <= 75) {
							sHealthColor.r = 255;
							sHealthColor.g = 255;
							sHealthColor.b = 0;
						}

						std::string szHealthText = std::to_string((_Longlong)pTempPlayerData->sEntInfo.m_iHealth) + "%";
						
						g_Objects.renderer.DrawString(szHealthText.c_str(), "Verdana", 7, 15, (int)pTempPlayerData->vScreenPos.x, (int)pTempPlayerData->vScreenPos.y + ucLineOffset, sHealthColor.r, sHealthColor.g, sHealthColor.b, sHealthColor.a);

						//ucLineOffset += LINE_OFFSET_ADD;
					}
				}
			}
		}
	}

	//Draw menu and cursor if visible
	if (g_pMenuForm->IsVisible()) {
		g_pMenuForm->Draw();

		DrawCursor();
	}

	//Handle menu toggle
	int iKeyCode = g_Objects.keys.GetKeyNum("KEY_INSERT");
	if (GetAsyncKeyState(iKeyCode)&1) {
		g_pMenuForm->ToggleVisibility();

		//Handle input activation
		SetMenuInputActivation(g_pMenuForm->IsVisible());

		PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "Menu is now %s\n", (g_pMenuForm->IsVisible()) ? "visible" : "invisible");
	}

	//End drawing state and present result
	if (!g_Objects.renderer.DrawEnd()) {
		FatalError("CRenderer::DrawEnd() failed: %d", GetLastError());
	}
}
//======================================================================

//======================================================================
void Main_Shutdown(void)
{
	//Hack shutdown function
	
	if (!bMainReady)
		return;

	//Inform thread that it shall now end
	if (!SetEvent(g_hThreadEndEvent))
		PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "Error: SetEvent() failed: %d\n", GetLastError());

	//Wait until thread function has finished
	WaitForSingleObject(g_hCollectThread, INFINITE);

	//Remove console control handler
	SetConsoleCtrlHandler(&HandlerRoutine, FALSE);

	//Close thread handle
	CloseHandle(g_hCollectThread);

	//Close event handles
	CloseHandle(g_hThreadStartEvent);
	CloseHandle(g_hThreadEndEvent);

	//Unregister low-level mouse hook if not already
	if (g_hMouseHook)
		UnhookWindowsHookEx(g_hMouseHook);

	//Unregister low-level keyboard hook if not already
	if (g_hKeyboardHook)
		UnhookWindowsHookEx(g_hKeyboardHook);

	bMainReady = false;
}
//======================================================================

//======================================================================
void ProgramMain(void)
{
	//Main program function implementation

	Main_Startup(); //Initialize all stuff
	if (Main_Ready()) { //If all is ready
		//Inform thread that it may now begin to collect the entity data
		if (!SetEvent(g_hThreadStartEvent))
			FatalError("Error: SetEvent() failed: %d", GetLastError());

		bMainLoopRunning = true; //Set main loop indicator to true (running)

		PrintConsole(CONSOLE_ATTRIBUTE_DEFAULT, "\nEverything seems to be fine, entering main loop\n\n");

		while (bMainLoopRunning) {
			g_Objects.window.Process(); //Process window stuff

			g_pMenuForm->Process(); //Process menu stuff

			//Handle stuff which is relevant only when the console window is in foreground
			if (IsValidWindow(g_hConsoleWindow)) {
				if (GetAsyncKeyState(VK_ESCAPE)&1)
					bMainLoopRunning = false;
			}

			//Check if game window is not valid anymore
			if (!IsWindow(g_hTargetWindow)) {
				bMainLoopRunning = false;
				return;
			}

			Sleep(1);
		}

		Main_Shutdown(); //Shutdown and cleanup
	}
}
//======================================================================

//======================================================================
#if defined(_BUILD_EXE)

int main(int argc, char* argv[])
{
	//Entrypoint for application build

	//Proceed with program main part
	ProgramMain();

	return EXIT_SUCCESS;
}

#elif defined(_BUILD_DLL)

DWORD WINAPI DllThreadProc(LPVOID lpvArguments)
{
	//DLL thread proc function

	//Proceed with program main part
	ProgramMain();

	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
	//Dll main entry point

	if (fdwReason == DLL_PROCESS_ATTACH) {
		//DLL is attached

		//Save module handle
		g_hOwnDllHandle = hModule;

		//Allocate console
		if (!AllocConsole())
			FatalError("Error: AllocConsole() failed for DLL build: %d", GetLastError());

		//Create thread to DLL thread procedure
		HANDLE hThread = CreateThread(NULL, 0, &DllThreadProc, NULL, 0, NULL);
		if (!hThread)
			FatalError("Error: CreateThread() failed for DLL build: %d", GetLastError());

		CloseHandle(hThread); //Close thread handle
	} else if (fdwReason == DLL_PROCESS_DETACH) {
		//DLL gets detached

		FreeConsole();
	}

	return TRUE;
}
#else
#error "Please specify either _BUILD_EXE or _BUILD_DLL"
#endif
//======================================================================
