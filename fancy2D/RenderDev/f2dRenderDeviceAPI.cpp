#include "f2dRenderDeviceAPI.h"

////////////////////////////////////////////////////////////////////////////////

f2dRenderDeviceAPI::f2dRenderDeviceAPI()
	: m_hModuleD3D9(NULL), m_hModuleD3DX9(NULL),
	m_Entry_pDirect3DCreate9(NULL),
	m_Entry_pD3DXSaveSurfaceToFileInMemory(NULL), m_Entry_pD3DXCreateFontW(NULL), m_Entry_pD3DXCreateTextureFromFileInMemoryEx(NULL)
{
	// 加载模块 D3D9
	m_hModuleD3D9 = LoadLibrary(L"D3D9.dll");
	if(!m_hModuleD3D9)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "LoadLibrary(D3D9.dll) Failed.");
	// 加载模块 D3DX9
	m_hModuleD3DX9 = LoadLibrary(L"D3DX9_43.dll");
	if(!m_hModuleD3DX9)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "LoadLibrary(D3DX9_43.dll) Failed.");
	// 获得入口 D3D9
	m_Entry_pDirect3DCreate9 = (pDLLEntry_Direct3DCreate9)GetProcAddress(m_hModuleD3D9, "Direct3DCreate9");
	if(!m_Entry_pDirect3DCreate9)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(Direct3DCreate9) Failed.");
	// 获得入口 D3DX9
	m_Entry_pD3DXSaveSurfaceToFileInMemory = (pDLLEntry_D3DXSaveSurfaceToFileInMemory)GetProcAddress(m_hModuleD3DX9, "D3DXSaveSurfaceToFileInMemory");
	if(!m_Entry_pD3DXSaveSurfaceToFileInMemory)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(D3DXSaveSurfaceToFileInMemory) Failed.");
	m_Entry_pD3DXCreateFontW = (pDLLEntry_D3DXCreateFontW)GetProcAddress(m_hModuleD3DX9, "D3DXCreateFontW");
	if(!m_Entry_pD3DXCreateFontW)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(D3DXCreateFontW) Failed.");
	m_Entry_pD3DXCreateTextureFromFileInMemoryEx = (pDLLEntry_D3DXCreateTextureFromFileInMemoryEx)GetProcAddress(m_hModuleD3DX9, "D3DXCreateTextureFromFileInMemoryEx");
	if(!m_Entry_pD3DXCreateTextureFromFileInMemoryEx)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(D3DXCreateTextureFromFileInMemoryEx) Failed.");
	m_Entry_pD3DXCreateEffectEx = (pDLLEntry_D3DXCreateEffectEx)GetProcAddress(m_hModuleD3DX9, "D3DXCreateEffectEx");
	if(!m_Entry_pD3DXCreateEffectEx)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(D3DXCreateEffectEx) Failed.");
	m_Entry_pD3DXCreateEffectFromFileExW = (pDLLEntry_D3DXCreateEffectFromFileExW)GetProcAddress(m_hModuleD3DX9, "D3DXCreateEffectFromFileExW");
	if(!m_Entry_pD3DXCreateEffectFromFileExW)
		throw fcyWin32Exception("f2dRenderDeviceAPI::f2dRenderDeviceAPI", "GetProcAddress(D3DXCreateEffectFromFileExW) Failed.");
}

f2dRenderDeviceAPI::~f2dRenderDeviceAPI()
{
}

IDirect3D9* f2dRenderDeviceAPI::DLLEntry_Direct3DCreate9(UINT SDKVersion)
{
	return m_Entry_pDirect3DCreate9(SDKVersion);
}

HRESULT f2dRenderDeviceAPI::DLLEntry_D3DXSaveSurfaceToFileInMemory(
	LPD3DXBUFFER*             ppDestBuf,
	D3DXIMAGE_FILEFORMAT      DestFormat,
	LPDIRECT3DSURFACE9        pSrcSurface,
	CONST PALETTEENTRY*       pSrcPalette,
	CONST RECT*               pSrcRect)
{
	return m_Entry_pD3DXSaveSurfaceToFileInMemory(ppDestBuf, DestFormat, pSrcSurface, pSrcPalette, pSrcRect);
}

HRESULT f2dRenderDeviceAPI::DLLEntry_D3DXCreateFontW(
	LPDIRECT3DDEVICE9       pDevice,  
	INT                     Height,
	UINT                    Width,
	UINT                    Weight,
	UINT                    MipLevels,
	BOOL                    Italic,
	DWORD                   CharSet,
	DWORD                   OutputPrecision,
	DWORD                   Quality,
	DWORD                   PitchAndFamily,
	LPCWSTR                 pFaceName,
	LPD3DXFONT*             ppFont)
{
	return m_Entry_pD3DXCreateFontW(pDevice, Height, Width, Weight, MipLevels, Italic, CharSet, OutputPrecision, Quality, PitchAndFamily, pFaceName, ppFont);
}

HRESULT f2dRenderDeviceAPI::DLLEntry_D3DXCreateTextureFromFileInMemoryEx(
	LPDIRECT3DDEVICE9         pDevice,
	LPCVOID                   pSrcData,
	UINT                      SrcDataSize,
	UINT                      Width,
	UINT                      Height,
	UINT                      MipLevels,
	DWORD                     Usage,
	D3DFORMAT                 Format,
	D3DPOOL                   Pool,
	DWORD                     Filter,
	DWORD                     MipFilter,
	D3DCOLOR                  ColorKey,
	D3DXIMAGE_INFO*           pSrcInfo,
	PALETTEENTRY*             pPalette,
	LPDIRECT3DTEXTURE9*       ppTexture)
{
	return m_Entry_pD3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}

HRESULT f2dRenderDeviceAPI::DLLEntry_D3DXCreateEffectEx(
	LPDIRECT3DDEVICE9               pDevice,
	LPCVOID                         pSrcData,
	UINT                            SrcDataLen,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	LPCSTR                          pSkipConstants, 
	DWORD                           Flags,
	LPD3DXEFFECTPOOL                pPool,
	LPD3DXEFFECT*                   ppEffect,
	LPD3DXBUFFER*                   ppCompilationErrors)
{
	return m_Entry_pD3DXCreateEffectEx(pDevice, pSrcData, SrcDataLen, pDefines, pInclude,pSkipConstants, Flags, pPool, ppEffect, ppCompilationErrors);
}

HRESULT f2dRenderDeviceAPI::DLLEntry_D3DXCreateEffectFromFileExW(
        LPDIRECT3DDEVICE9               pDevice,
        LPCWSTR                         pSrcFile,
        CONST D3DXMACRO*                pDefines,
        LPD3DXINCLUDE                   pInclude,
        LPCSTR                          pSkipConstants, 
        DWORD                           Flags,
        LPD3DXEFFECTPOOL                pPool,
        LPD3DXEFFECT*                   ppEffect,
        LPD3DXBUFFER*                   ppCompilationErrors)
{
	return m_Entry_pD3DXCreateEffectFromFileExW(pDevice, pSrcFile, pDefines, pInclude, pSkipConstants, Flags, pPool, ppEffect, ppCompilationErrors);
}
