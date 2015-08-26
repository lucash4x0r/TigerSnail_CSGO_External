#ifndef _RENDERER_H
#define _RENDERER_H

#include "includes.h"
#include <d3d9.h>
#include <d3dx9core.h>
#include <DxErr.h>

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: renderer.h: Renderer interface
*/

//======================================================================
#define INVALID_LIST_ID std::string::npos
//======================================================================

//======================================================================
struct drawinginterface_s {
	BOOL (WINAPI *TpfnPrintText)(LPCSTR lpszMessage);
	BOOL (WINAPI *TpfnDrawText)(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL (WINAPI *TpfnDrawBox)(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL (WINAPI *TpfnDrawFilledBox)(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL (WINAPI *TpfnDrawLine)(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a);
	size_t (WINAPI *TpfnLoadImage)(LPCSTR lpszFileName);
	BOOL (WINAPI *TpfnDrawImage)(const size_t uiImageId, INT x, INT y);
};

struct d3dfont_s {
	LPD3DXFONT pFont; //Pointer to font object
	CHAR szFontName[LF_FACESIZE]; //Font name string
	BYTE ucFontSizeW, ucFontSizeH; //Font size
};

struct d3dimage_s {
	std::string szFile; //Full file name
	LPDIRECT3DSURFACE9 pSurface; //Surface data pointer
	D3DXIMAGE_INFO d3dImageInfo; //Image info
};
//======================================================================

//======================================================================
class CRenderer {
private:
	std::string m_szImagePath;

	HWND m_hWnd;
	INT m_iWidth, m_iHeight;

	IDirect3D9* m_pInterface;
	IDirect3DDevice9* m_pDevice;
	IDirect3DSurface9* m_pBackBuffer;
	IDirect3DSurface9* m_pImageSurface;

	D3DCOLOR m_d3dcClearColor;

	std::vector<d3dfont_s*> m_vFonts;
	std::vector<d3dimage_s*> m_vImages;

	d3dfont_s* GetExistingFont(LPCSTR lpszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH);
	d3dfont_s* AddFont(LPCSTR lpszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH);

	inline bool IsValidImageId(const size_t uiImageId) { return (uiImageId != INVALID_LIST_ID) && (uiImageId < this->m_vImages.size()); }
public:
	CRenderer() : m_hWnd(0), m_iWidth(0), m_iHeight(0), m_pInterface(NULL), m_pDevice(NULL), m_pBackBuffer(NULL), m_pImageSurface(NULL) { }
	~CRenderer() { }

	BOOL Initialize(HWND hWnd, BOOL bWindowed, INT iWidth, INT iHeight, BYTE r, BYTE g, BYTE b, BYTE a, LPCSTR lpszAbsImagePath);
	VOID Clear(VOID);

	BOOL DrawBegin(VOID);
	BOOL DrawEnd(VOID);

	size_t LoadImage(LPCSTR lpszImage);

	BOOL DrawString(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL DrawBox(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL DrawFilledBox(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL DrawLine(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a);
	BOOL DrawImage(const size_t uiImageId, INT x, INT y);

	BOOL StoreScreenshotToDisk(LPCSTR lpszTargetFile);

	VOID SetClearColor(D3DCOLOR d3dcColor);
	D3DCOLOR GetClearColor(VOID);

	BOOL SetBackgroundPicture(LPCSTR lpszPictureFile);
};
//======================================================================

#endif
