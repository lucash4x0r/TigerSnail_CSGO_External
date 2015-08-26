#include "renderer.h"
#include "utils.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: renderer.cpp: Renderer implementations
*/

//======================================================================
BOOL CRenderer::Initialize(HWND hWnd, BOOL bWindowed, INT iWidth, INT iHeight, BYTE r, BYTE g, BYTE b, BYTE a, LPCSTR lpszAbsImagePath)
{
	//Initialize DirectX
	
	if ((!hWnd) || (!iWidth) || (!iHeight))
		return FALSE;
	
	//Create D3D9 interface
	this->m_pInterface = Direct3DCreate9(D3D_SDK_VERSION);
	if (!this->m_pInterface)
		return FALSE;
	
	//Setup data
	D3DPRESENT_PARAMETERS d3dppParameters;
	memset(&d3dppParameters, 0x00, sizeof(d3dppParameters));
	d3dppParameters.Windowed = bWindowed; //Windowed or fullscreened target window
	d3dppParameters.SwapEffect = D3DSWAPEFFECT_DISCARD; //Driver shall show backbuffer via the most fast way
	d3dppParameters.hDeviceWindow = hWnd; //Handle of target window
	d3dppParameters.BackBufferFormat = D3DFMT_A8R8G8B8; //Use 32-bit color depth (RGBA)
	d3dppParameters.BackBufferWidth = iWidth; //Width of backbuffer
	d3dppParameters.BackBufferHeight = iHeight; //Height of backbuffer
	
	//Create device
	if (FAILED(this->m_pInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dppParameters, &this->m_pDevice))) {
		this->m_pInterface->Release();
		this->m_pInterface = NULL;
		return FALSE;
	}
	
	//Get pointer to backbuffer
	if (FAILED(this->m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->m_pBackBuffer))) {
		this->m_pDevice->Release();
		this->m_pInterface->Release();
		this->m_pDevice = NULL;
		this->m_pInterface = NULL;
		return FALSE;
	}
	
	//Save data
	this->m_szImagePath = lpszAbsImagePath;
	this->m_hWnd = hWnd;
	this->m_iWidth = iWidth;
	this->m_iHeight = iHeight;
	this->m_d3dcClearColor = D3DCOLOR_ARGB(a, r, g ,b);

	return TRUE;
}
//======================================================================

//======================================================================
VOID CRenderer::Clear(VOID)
{
	//Clear data and free memory

	if (!this->m_pInterface)
		return;

	//Clear fonts
	
	for (size_t i = 0; i < this->m_vFonts.size(); i++) {
		if (this->m_vFonts[i]) {
			this->m_vFonts[i]->pFont->Release(); //Release object
			delete this->m_vFonts[i]; //Free memory
		}
	}

	this->m_vFonts.clear(); //Clear list

	//Clear images

	for (size_t i = 0; i < this->m_vImages.size(); i++) {
		if (this->m_vImages[i]) {
			this->m_vImages[i]->pSurface->Release(); //Release object
			delete this->m_vImages[i]; //Free memory
		}
	}

	this->m_vImages.clear(); //Clear list

	//Release device
	if (this->m_pDevice)
		this->m_pDevice->Release();

	//Release interface
	this->m_pInterface->Release();

	//Clear data
	this->m_pInterface = NULL;
	this->m_pDevice = NULL;
	this->m_hWnd = 0;
	this->m_iWidth = this->m_iHeight = 0;
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawBegin(VOID)
{
	//Begin scene drawing

	if ((!this->m_pInterface) || (!this->m_pDevice))
		return FALSE;

	//Clear backbuffer
	if (FAILED(this->m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, this->m_d3dcClearColor, 0, 0)))
		return FALSE;

	//Begin scene
	if (FAILED(this->m_pDevice->BeginScene()))
		return FALSE;

	//Check for background image
	if (this->m_pImageSurface) {
		//Set RECT data
		RECT rSrc;
		rSrc.left = 0;
		rSrc.top = 0;
		rSrc.right = this->m_iWidth;
		rSrc.bottom = this->m_iHeight;

		return this->m_pDevice->StretchRect(this->m_pImageSurface, &rSrc, this->m_pBackBuffer, NULL, D3DTEXF_NONE); //Copy surface into backbuffer
	}

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawEnd(VOID)
{
    //End scene drawing
 
	if ((!this->m_pInterface) || (!this->m_pDevice))
		return FALSE;
 
	//End scene
	if (FAILED(this->m_pDevice->EndScene()))
		return FALSE;
 
	//Copy backbuffer into frontbuffer
	return SUCCEEDED(this->m_pDevice->Present(NULL, NULL, this->m_hWnd, NULL));
}
//======================================================================

//======================================================================
d3dfont_s* CRenderer::GetExistingFont(LPCSTR lpszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH)
{
	//Get font data pointer

	if (!lpszFontName)
		return NULL;

	//Search in list
	for (size_t i = 0; i < this->m_vFonts.size(); i++) {
		if ((this->m_vFonts[i]->ucFontSizeW == ucFontSizeW) && (this->m_vFonts[i]->ucFontSizeH == ucFontSizeH) && (strcmp(this->m_vFonts[i]->szFontName, lpszFontName)==0)) { //Compare data
			return this->m_vFonts[i];
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
d3dfont_s* CRenderer::AddFont(LPCSTR lpszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH)
{
	//Create new font

	if (!lpszFontName)
		return NULL;

	d3dfont_s* pFont = NULL;

	//Check if already exists
	pFont = this->GetExistingFont(lpszFontName, ucFontSizeW, ucFontSizeH);
	if (pFont)
		return pFont;

	//Allocate memory font font structure
	pFont = new d3dfont_s;
	if (!pFont)
		return NULL;

	//Create font object
	if (FAILED(D3DXCreateFontA(this->m_pDevice, (INT)ucFontSizeH, (UINT)(ucFontSizeW), FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, lpszFontName, &pFont->pFont))) {
		delete [] pFont;
		return NULL;
	}

	//Set data
	strcpy_s(pFont->szFontName, lpszFontName);
	pFont->ucFontSizeW = ucFontSizeW;
	pFont->ucFontSizeH = ucFontSizeH;

	//Add to list
	this->m_vFonts.push_back(pFont);

	return pFont;
}
//======================================================================

//======================================================================
size_t CRenderer::LoadImage(LPCSTR lpszImage)
{
	//Load image

	if (!lpszImage)
		return INVALID_LIST_ID;

	//Setup full file path
	std::string szAbsImagePath = this->m_szImagePath + "\\" + lpszImage;

	//Allocate memory
	d3dimage_s* pImageData = new d3dimage_s;
	if (!pImageData)
		return INVALID_LIST_ID;
	
	//Get image info
	if (FAILED(D3DXGetImageInfoFromFileA(szAbsImagePath.c_str(), &pImageData->d3dImageInfo))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}
	
	//Create surface for image
	if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(pImageData->d3dImageInfo.Width, pImageData->d3dImageInfo.Height, pImageData->d3dImageInfo.Format, D3DPOOL_DEFAULT, &pImageData->pSurface, NULL))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}

	//Load image into surface
	if (FAILED(D3DXLoadSurfaceFromFileA(pImageData->pSurface, NULL, NULL, szAbsImagePath.c_str(), NULL, D3DX_DEFAULT, 0, &pImageData->d3dImageInfo))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}

	//Save file name
	pImageData->szFile = szAbsImagePath;

	//Add to list
	this->m_vImages.push_back(pImageData);

	return this->m_vImages.size() - 1; //Return item ID
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawString(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a)
{
	//Draw a string on backbuffer

	if ((!this->m_pDevice) || (!lpszText) || (!lpszFont))
		return FALSE;

	//Get font or make one

	d3dfont_s* pFont;

	pFont = this->GetExistingFont(lpszFont, ucFontSizeW, ucFontSizeH);
	if (!pFont) {
		pFont = this->AddFont(lpszFont, ucFontSizeW, ucFontSizeH);
		if (!pFont)
			return FALSE;
	}

	//Set data
	RECT rect = {x, y, x, y};
	D3DCOLOR d3dcColor = D3DCOLOR_ARGB(a, r, g, b);

	//Calculate font rect
	INT iResult = pFont->pFont->DrawTextA(NULL, lpszText, (INT)strlen(lpszText), &rect, DT_NOCLIP, d3dcColor);

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawBox(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a)
{
	//Draw a box skeleton on backbuffer

	if (!this->m_pDevice)
		return FALSE;

	if (!DrawFilledBox(x, y, w, iThickness, r, g, b, a))
		return FALSE;

	if (!DrawFilledBox(x, y, iThickness, h, r, g, b, a))
		return FALSE;

	if (!DrawFilledBox(x + w, y, iThickness, h + iThickness, r, g, b, a))
		return FALSE;

	return DrawFilledBox(x, y + h, w, iThickness, r, g, b, a);
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawFilledBox(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	//Draw a filled box on backbuffer

	if (!this->m_pDevice)
		return FALSE;

	IDirect3DSurface9* pSurface = NULL;

    //Create surface
	if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(w, h, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSurface, NULL)))
		return FALSE;

	//Fill surface with color
	if (FAILED(this->m_pDevice->ColorFill(pSurface, NULL, D3DCOLOR_ARGB(a, r, g, b)))) {
		pSurface->Release();
		return FALSE;
	}

	//Set destination area
	RECT rTargetRect;
	rTargetRect.left = x;
	rTargetRect.top = y;
	rTargetRect.right = x + w;
	rTargetRect.bottom = y + h;

	//Copy surface into backbuffer at desired position
	BOOL bResult = SUCCEEDED(this->m_pDevice->StretchRect(pSurface, NULL, this->m_pBackBuffer, &rTargetRect, D3DTEXF_NONE));
	
	//Release surface
	pSurface->Release();

	return bResult;
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawLine(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a)
{
	//Draw a line on backbuffer

	if (!this->m_pDevice)
		return FALSE;

	//Define custom vertex type
	struct D3DVERTEX {
		float x, y, z, rhw;
		D3DCOLOR d3dColor;
	};

	//Setup vertex buffer
	D3DVERTEX vVertices[2] = {
		{(float)x1, (float)y1, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b)}, //Line source point
		{(float)x2, (float)y2, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b)} //Line destination point
	};
	
	//Set custom vertex format (Vertex has x, y, z, rhw and color member)
	if (FAILED(this->m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)))
		return FALSE;

	//Draw line primitive
	return this->m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, sizeof(vVertices) / sizeof(D3DVERTEX), vVertices, sizeof(D3DVERTEX)) == D3D_OK;
}
//======================================================================

//======================================================================
BOOL CRenderer::DrawImage(const size_t uiImageId, INT x, INT y)
{
	//Draw image from list

	//Check image ID for validity
	if (!IsValidImageId(uiImageId))
		return FALSE;

	if (!this->m_vImages[uiImageId])
		return FALSE;
	
	//Setup rect data
	RECT rDstRect;
	rDstRect.top = y;
	rDstRect.left = x;
	rDstRect.right = x + this->m_vImages[uiImageId]->d3dImageInfo.Width;
	rDstRect.bottom = y + this->m_vImages[uiImageId]->d3dImageInfo.Height;
	
	//Copy image surface to backbuffer
	return SUCCEEDED(this->m_pDevice->StretchRect(this->m_vImages[uiImageId]->pSurface, NULL, this->m_pBackBuffer, &rDstRect, D3DTEXF_NONE));
}
//======================================================================

//======================================================================
BOOL CRenderer::StoreScreenshotToDisk(LPCSTR lpszTargetFile)
{
	//Store a screenshot file with content of the frontbuffer

	if ((!lpszTargetFile) || (!this->m_pInterface) || (!this->m_pDevice))
		return FALSE;

	IDirect3DSurface9* pSurface = NULL;

	//Create surface
	if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(this->m_iWidth, this->m_iHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, NULL)))
		return FALSE;

	//Copy frontbuffer content into surface
	if (FAILED(this->m_pDevice->GetFrontBufferData(0, pSurface))) {
		pSurface->Release();
		return FALSE;
	}

	//Save surface content to file with Bitmap format
	if (FAILED(D3DXSaveSurfaceToFileA(lpszTargetFile, D3DXIFF_BMP, pSurface, NULL, NULL))) {
		pSurface->Release();
		return FALSE;
	}

	//Release surface
	pSurface->Release();

	return TRUE;
}
//======================================================================

//======================================================================
VOID CRenderer::SetClearColor(D3DCOLOR d3dcColor)
{
	//Set new clear color

	this->m_d3dcClearColor = d3dcColor; 
}
//======================================================================

//======================================================================
D3DCOLOR CRenderer::GetClearColor(VOID)
{
	//Get current clear color

	return this->m_d3dcClearColor; 
}
//======================================================================

//======================================================================
BOOL CRenderer::SetBackgroundPicture(LPCSTR lpszPictureFile)
{
	//Set background image

	if (!lpszPictureFile) { //Clear picture
		if (this->m_pImageSurface) {
			this->m_pImageSurface->Release(); //Release surface
			this->m_pImageSurface = NULL; //Set to NULL
		}
	} else { //Create new surface with picture
		D3DXIMAGE_INFO d3dimgInfo;
		if (FAILED(D3DXGetImageInfoFromFileA(lpszPictureFile, &d3dimgInfo))) //Get image info
			return FALSE;

		if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(d3dimgInfo.Width, d3dimgInfo.Height, d3dimgInfo.Format, D3DPOOL_DEFAULT, &this->m_pImageSurface, NULL))) //Create surface for image
			return FALSE;

		if (FAILED(D3DXLoadSurfaceFromFileA(this->m_pImageSurface, NULL, NULL, lpszPictureFile, NULL, D3DX_DEFAULT, 0, &d3dimgInfo))) { //Load image into surface
			this->m_pImageSurface->Release();
			this->m_pImageSurface = NULL;
			return FALSE;
		}
	}

	return TRUE;
}
//======================================================================
