#include "f2dRenderDeviceImpl.h"

#include "f2dEngineImpl.h"

#include "fcyException.h"

#include "fcyMisc/fcyStringHelper.h"

using namespace std;

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

////////////////////////////////////////////////////////////////////////////////

f2dTextureStatic::f2dTextureStatic(IDirect3DTexture9* pTex, fuInt Width, fuInt Height)
	: m_pTex(pTex), m_Width(Width), m_Height(Height)
{}

f2dTextureStatic::~f2dTextureStatic()
{
	FCYSAFEKILL(m_pTex);
}

IDirect3DBaseTexture9* f2dTextureStatic::GetHandle()
{
	return m_pTex;
}

fBool f2dTextureStatic::IsDynamic()
{
	return false;
}

fBool f2dTextureStatic::IsRenderTarget()
{
	return false;
}

fuInt f2dTextureStatic::GetWidth()
{
	return m_Width;
}

fuInt f2dTextureStatic::GetHeight()
{
	return m_Height;
}

fResult f2dTextureStatic::Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)
{
	if(pOut)
		*pOut = NULL;
	
	return FCYERR_NOTSUPPORT;
}

fResult f2dTextureStatic::Unlock()
{
	return FCYERR_NOTSUPPORT;
}

////////////////////////////////////////////////////////////////////////////////

f2dTextureDynamic::f2dTextureDynamic(f2dRenderDeviceImpl* pDev, IDirect3DTexture9* pTex, fuInt Width, fuInt Height)
	: m_pDev(pDev), m_pTex(pTex), m_Width(Width), m_Height(Height)
{
	m_pDev->AttachListener(this);
}

f2dTextureDynamic::~f2dTextureDynamic()
{
	m_pDev->RemoveListener(this);

	FCYSAFEKILL(m_pTex);
}

void f2dTextureDynamic::OnRenderDeviceLost()
{
	FCYSAFEKILL(m_pTex);
}

void f2dTextureDynamic::OnRenderDeviceReset()
{
	((IDirect3DDevice9*)m_pDev->GetHandle())->CreateTexture(m_Width, m_Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTex, NULL);
}

IDirect3DBaseTexture9* f2dTextureDynamic::GetHandle()
{
	return m_pTex;
}

fBool f2dTextureDynamic::IsDynamic()
{
	return true;
}

fBool f2dTextureDynamic::IsRenderTarget()
{
	return false;
}

fuInt f2dTextureDynamic::GetWidth()
{
	return m_Width;
}

fuInt f2dTextureDynamic::GetHeight()
{
	return m_Height;
}

fResult f2dTextureDynamic::Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)
{
	if(pOut)
		*pOut = NULL;
	if(!m_pTex)
		return FCYERR_INTERNALERR;
	if(Pitch)
		*Pitch = 0;


	RECT tRectToLock;
	D3DLOCKED_RECT tRectLocked;

	if(pLockRect)
	{
		tRectToLock.left = (int)pLockRect->a.x;
		tRectToLock.top = (int)pLockRect->a.y;
		tRectToLock.right = (int)pLockRect->b.x;
		tRectToLock.bottom = (int)pLockRect->b.y;
	}

	if(FAILED(m_pTex->LockRect(0, &tRectLocked, (pLockRect ? &tRectToLock : NULL), Discard ? D3DLOCK_DISCARD : 0)))
	{
		return FCYERR_INTERNALERR;
	}
	else
	{
		*Pitch = tRectLocked.Pitch;
		*pOut = ((fData)tRectLocked.pBits);

		return FCYERR_OK;
	}
}

fResult f2dTextureDynamic::Unlock()
{
	if(!m_pTex)
		return FCYERR_INTERNALERR;

	if(FAILED(m_pTex->UnlockRect(0)))
	{
		return FCYERR_INTERNALERR;
	}
	else
	{
		return FCYERR_OK;
	}
}

////////////////////////////////////////////////////////////////////////////////
	
f2dTextureRenderTarget::f2dTextureRenderTarget(f2dRenderDeviceImpl* pDev, IDirect3DTexture9* pTex, fuInt Width, fuInt Height)
	: m_pDev(pDev), m_pTex(pTex), m_pSurface(NULL), m_Width(Width), m_Height(Height)
{
	m_pDev->AttachListener(this);

	m_pTex->GetSurfaceLevel(0, &m_pSurface);
}

f2dTextureRenderTarget::~f2dTextureRenderTarget()
{
	m_pDev->RemoveListener(this);

	FCYSAFEKILL(m_pSurface);
	FCYSAFEKILL(m_pTex);
}

IDirect3DSurface9* f2dTextureRenderTarget::GetSurface()
{
	return m_pSurface;
}

void f2dTextureRenderTarget::OnRenderDeviceLost()
{
	FCYSAFEKILL(m_pSurface);
	FCYSAFEKILL(m_pTex);
}

void f2dTextureRenderTarget::OnRenderDeviceReset()
{
	((IDirect3DDevice9*)m_pDev->GetHandle())->CreateTexture(m_Width, m_Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTex, NULL);
	m_pTex->GetSurfaceLevel(0, &m_pSurface);
}

IDirect3DBaseTexture9* f2dTextureRenderTarget::GetHandle()
{
	return m_pTex;
}

fBool f2dTextureRenderTarget::IsDynamic()
{
	return false;
}

fBool f2dTextureRenderTarget::IsRenderTarget()
{
	return true;
}

fuInt f2dTextureRenderTarget::GetWidth()
{
	return m_Width;
}

fuInt f2dTextureRenderTarget::GetHeight()
{
	return m_Height;
}

fResult f2dTextureRenderTarget::Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut)
{
	if(pOut)
		*pOut = NULL;

	return FCYERR_NOTSUPPORT;
}

fResult f2dTextureRenderTarget::Unlock()
{
	return FCYERR_NOTSUPPORT;
}

////////////////////////////////////////////////////////////////////////////////

const fuInt f2dGraphics2DImpl::ConstantConvert[F2DRENDERSTATE_MASK] = 
{
	D3DRS_BLENDOP,
	D3DRS_SRCBLEND,
	D3DRS_DESTBLEND,
	D3DTSS_COLOROP,
	D3DTSS_COLORARG1,
	D3DTSS_COLORARG2,
	D3DTSS_ALPHAOP,
	D3DTSS_ALPHAARG1,
	D3DTSS_ALPHAARG2
};

const fuInt f2dGraphics2DImpl::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

f2dGraphics2DImpl::f2dGraphics2DImpl(f2dRenderDeviceImpl* pDev, fuInt VertexBufferCount, fuInt IndexBufferCount)
	: m_pParentDev(pDev),
	m_RenderStart(false), m_VBUsedCount(0), m_IBUsedCount(0), m_pVB(NULL), m_pIB(NULL),
	m_VBMaxCount(VertexBufferCount), m_IBMaxCount(IndexBufferCount)
{
	m_pDev = (IDirect3DDevice9*)m_pParentDev->GetHandle();
	
	m_CurProj = fcyMatrix4::GetOrthoOffCenterLH(0.f, (float)pDev->GetBufferWidth(), (float)pDev->GetBufferHeight(), 0.f, 0.f, 100.f);

	// 设置默认状态
	m_CurStates[F2DRENDERSTATE_BLENDOPT] = F2DBLENDOPERATOR_ADD;
	m_CurStates[F2DRENDERSTATE_BLENDSRC] = F2DBLENDFACTOR_SRCALPHA;
	m_CurStates[F2DRENDERSTATE_BLENDDEST] = F2DBLENDFACTOR_INVSRCALPHA;
	m_CurStates[F2DRENDERSTATE_TEXCOLOROPT] = F2DTEXOPT_ADD;
	m_CurStates[F2DRENDERSTATE_TEXCOLORARG1] = F2DTEXARG_DIFFUSE;
	m_CurStates[F2DRENDERSTATE_TEXCOLORARG2] = F2DTEXARG_TEXTURE;
	m_CurStates[F2DRENDERSTATE_TEXALPHAOPT] = F2DTEXOPT_MODULATE;
	m_CurStates[F2DRENDERSTATE_TEXALPHAARG1] = F2DTEXARG_DIFFUSE;
	m_CurStates[F2DRENDERSTATE_TEXALPHAARG2] = F2DTEXARG_TEXTURE;

	HRESULT tHR;
	if(FAILED(tHR = m_pDev->CreateVertexBuffer(m_VBMaxCount * sizeof(f2dGraphics2DVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		throw fcyWin32COMException("f2dGraphics2DImpl::f2dGraphics2DImpl", "CreateVertexBuffer Failed.", tHR);
	if(FAILED(tHR = m_pDev->CreateIndexBuffer(m_IBMaxCount * sizeof(fuShort), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		FCYSAFEKILL(m_pVB);
		throw fcyWin32COMException("f2dGraphics2DImpl::f2dGraphics2DImpl", "CreateIndexBuffer Failed.", tHR);
	}

	m_pParentDev->AttachListener(this);
}

f2dGraphics2DImpl::~f2dGraphics2DImpl()
{
	m_pParentDev->RemoveListener(this);

	FCYSAFEKILL(m_pVB);
	FCYSAFEKILL(m_pIB);
}

void f2dGraphics2DImpl::flush(bool Discard)
{
	if(m_Commands.empty())
	{
		if(Discard)
		{
			// 解锁缓冲区
			m_pVB->Unlock();
			m_pIB->Unlock();

			m_pVB->Lock(0, m_VBMaxCount * sizeof(f2dGraphics2DVertex), (void**)&m_pVBData, D3DLOCK_DISCARD);
			m_pIB->Lock(0, m_IBMaxCount * sizeof(fuShort), (void**)&m_pIBData, D3DLOCK_DISCARD);
			m_VBUsedCount = 0;
			m_IBUsedCount = 0;
			m_VBAlloced = 0;
			m_IBAlloced = 0;
		}
		else
			return;
	}

	// 解锁缓冲区
	m_pVB->Unlock();
	m_pIB->Unlock();

	// 绘制并统计有效顶点个数
	vector<RenderCommand>::iterator i = m_Commands.begin();
	while(i != m_Commands.end())
	{
		if(i->pTex)
		{
			m_pDev->SetTexture(0, i->pTex->GetHandle());
			i->pTex->Release();
		}
		else
			m_pDev->SetTexture(0, NULL);

		m_pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_VBUsedCount, 0, i->VertCount, m_IBUsedCount, i->IndexCount/3);

		m_VBUsedCount += i->VertCount;
		m_IBUsedCount += i->IndexCount;

		i++;
	}

	m_Commands.clear();

	// 重新锁定缓冲区
	if(m_VBUsedCount == m_VBMaxCount || m_IBUsedCount == m_IBMaxCount)
		Discard = true;

	if(Discard)
	{
		m_pVB->Lock(0, m_VBMaxCount * sizeof(f2dGraphics2DVertex), (void**)&m_pVBData, D3DLOCK_DISCARD);
		m_pIB->Lock(0, m_IBMaxCount * sizeof(fuShort), (void**)&m_pIBData, D3DLOCK_DISCARD);
		m_VBUsedCount = 0;
		m_IBUsedCount = 0;
		m_VBAlloced = 0;
		m_IBAlloced = 0;
	}
	else
	{
		m_pVB->Lock(m_VBUsedCount * sizeof(f2dGraphics2DVertex), (m_VBMaxCount - m_VBUsedCount) * sizeof(f2dGraphics2DVertex), (void**)&m_pVBData, D3DLOCK_NOOVERWRITE);
		m_pIB->Lock(m_IBUsedCount * sizeof(fuShort), (m_IBMaxCount - m_IBUsedCount) * sizeof(fuShort), (void**)&m_pIBData, D3DLOCK_NOOVERWRITE);

		m_VBAlloced = 0;
		m_IBAlloced = 0;
	}
}

bool f2dGraphics2DImpl::alloc(fuInt VertCountToAlloc, fuInt IndexCountToAlloc, f2dGraphics2DVertex** pVertexOut, fuShort** pIndexOut)
{
	if(VertCountToAlloc > m_VBMaxCount || IndexCountToAlloc > m_IBMaxCount)
		return false;

	if((m_VBMaxCount - m_VBUsedCount - m_VBAlloced) < VertCountToAlloc || (m_IBMaxCount - m_IBUsedCount - m_IBAlloced) < IndexCountToAlloc)
		flush(true);

	*pVertexOut = (m_pVBData + m_VBAlloced);
	*pIndexOut = (m_pIBData + m_IBAlloced);

	m_VBAlloced += VertCountToAlloc;
	m_IBAlloced += IndexCountToAlloc;

	return true;
}

void f2dGraphics2DImpl::pushcommand(f2dTextureInternal* pTex, fuInt VertCount, fuInt IndexCount)
{
	if(!m_Commands.empty())
	{
		RenderCommand* pCmd = &m_Commands[m_Commands.size()-1];

		if(pCmd->pTex == pTex)
		{
			// 调整索引
			fuShort* pIndex = (m_pIBData + m_IBAlloced - IndexCount);
			for(fuInt i = 0; i<IndexCount; i++)
				pIndex[i] += pCmd->VertCount;

			pCmd->VertCount += VertCount;
			pCmd->IndexCount += IndexCount;
			return;
		}
	}

	RenderCommand tCmd = { pTex, VertCount, IndexCount };
	
	if(pTex)
		pTex->AddRef();
	m_Commands.push_back(tCmd);
}

void f2dGraphics2DImpl::OnRenderDeviceLost()
{
	FCYSAFEKILL(m_pVB);
	FCYSAFEKILL(m_pIB);
}	

void f2dGraphics2DImpl::OnRenderDeviceReset()
{
	m_pDev->CreateVertexBuffer(m_VBMaxCount * sizeof(f2dGraphics2DVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	m_pDev->CreateIndexBuffer(m_IBMaxCount * sizeof(fuShort), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
}

fuInt f2dGraphics2DImpl::GetRenderState(F2DRENDERSTATE State)
{
	if(State >= F2DRENDERSTATE_MASK)
		return 0;
	else
		return m_CurStates[State];
}

void f2dGraphics2DImpl::SetRenderState(F2DRENDERSTATE State, fuInt Value)
{
	if(State >= F2DRENDERSTATE_MASK)
		return;

	m_CurStates[State] = Value;
	
	if(m_RenderStart)
	{
		flush();

		if(State <= F2DRENDERSTATE_BLENDDEST) 
			m_pDev->SetRenderState((D3DRENDERSTATETYPE)ConstantConvert[State], Value);
		else
			m_pDev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)ConstantConvert[State], Value);
	}
}

void f2dGraphics2DImpl::GetProjTransform(fcyMatrix4* pOut)
{
	if(pOut)
		*pOut = m_CurProj;
}

void f2dGraphics2DImpl::SetProjTransform(const fcyMatrix4& pTrans)
{
	m_CurProj = pTrans;

	if(m_RenderStart)
	{
		flush();

		m_pDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_CurProj);
	}
}

void f2dGraphics2DImpl::GetWorldTransform(fcyMatrix4* pOut)
{
	if(pOut)
		*pOut = m_CurMatrix;
}

void f2dGraphics2DImpl::SetWorldTransform(const fcyMatrix4& pTrans)
{
	m_CurMatrix = pTrans;

	if(m_RenderStart)
	{
		flush();

		m_pDev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_CurMatrix);
	}
}

void f2dGraphics2DImpl::PushWorldTransform()
{
	m_MatList.push(m_CurMatrix);
}

void f2dGraphics2DImpl::PopWorldTransform()
{
	if(!m_MatList.empty())
	{
		SetWorldTransform(m_MatList.top());
		m_MatList.pop();
	}
}

void f2dGraphics2DImpl::SyncStates()
{
	// 同步所有渲染状态
	for(int i = F2DRENDERSTATE_BLENDOPT; i<=F2DRENDERSTATE_BLENDDEST; i++)
		m_pDev->SetRenderState((D3DRENDERSTATETYPE)ConstantConvert[i], m_CurStates[i]);
	for(int i = F2DRENDERSTATE_TEXCOLOROPT; i<=F2DRENDERSTATE_TEXALPHAARG2; i++)
		m_pDev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)ConstantConvert[i], m_CurStates[i]);
}

fResult f2dGraphics2DImpl::Begin()
{
	if(m_RenderStart)
		return FCYERR_ILLEGAL;

	if(!(m_pIB && m_pVB))
		return FCYERR_INTERNALERR;

	m_pDev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_CurMatrix);
	m_pDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_CurProj);

	m_pDev->SetFVF(FVF);
	m_pDev->SetStreamSource(0, m_pVB, 0, sizeof(f2dGraphics2DVertex));
	m_pDev->SetIndices(m_pIB);

	if(FAILED(m_pDev->BeginScene()))
	{
		m_RenderStart = false;
		return FCYERR_INTERNALERR;
	}
	else
	{
		m_RenderStart = true;

		// 准备缓冲区
		m_pVB->Lock(0, m_VBMaxCount*sizeof(f2dGraphics2DVertex), (void**)&m_pVBData, D3DLOCK_DISCARD);
		m_pIB->Lock(0, m_IBMaxCount*sizeof(fuShort), (void**)&m_pIBData, D3DLOCK_DISCARD);

		m_VBUsedCount = 0;
		m_IBUsedCount = 0;
		m_VBAlloced = 0;
		m_IBAlloced = 0;

		return FCYERR_OK;
	}
}

fResult f2dGraphics2DImpl::End()
{
	if(!m_RenderStart)
		return FCYERR_ILLEGAL;

	flush();

	m_RenderStart = false;

	m_pVB->Unlock();
	m_pIB->Unlock();

	if(FAILED(m_pDev->EndScene()))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fResult f2dGraphics2DImpl::Flush()
{
	if(!m_RenderStart)
		return FCYERR_ILLEGAL;

	flush();
	return FCYERR_OK;
}

fResult f2dGraphics2DImpl::DrawQuad(f2dTexture* pTex,
				 const f2dGraphics2DVertex& v1, 
				 const f2dGraphics2DVertex& v2, 
				 const f2dGraphics2DVertex& v3, 
				 const f2dGraphics2DVertex& v4
				 )
{
	if(!m_RenderStart)
		return FCYERR_ILLEGAL;

	f2dGraphics2DVertex* pVerts;
	fuShort* pIndex;
	if(!alloc(4, 6, &pVerts, &pIndex))
		return FCYERR_INVAILDPARAM;

	pVerts[0] = v1;
	pVerts[1] = v2;
	pVerts[2] = v3;
	pVerts[3] = v4;

	pVerts[0].x += 0.5f;
	pVerts[0].y += 0.5f;
	pVerts[1].x += 0.5f;
	pVerts[1].y += 0.5f;
	pVerts[2].x += 0.5f;
	pVerts[2].y += 0.5f;
	pVerts[3].x += 0.5f;
	pVerts[3].y += 0.5f;
	
	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 0;
	pIndex[4] = 2;
	pIndex[5] = 3;

	pushcommand((f2dTextureInternal*)pTex, 4, 6);

	return FCYERR_OK;
}

fResult f2dGraphics2DImpl::DrawQuad(f2dTexture* pTex, const f2dGraphics2DVertex* arr)
{
	if(!m_RenderStart)
		return FCYERR_ILLEGAL;

	f2dGraphics2DVertex* pVerts;
	fuShort* pIndex;
	if(!alloc(4, 6, &pVerts, &pIndex))
		return FCYERR_INVAILDPARAM;

	memcpy(pVerts, arr, sizeof(f2dGraphics2DVertex) * 4);
	
	pVerts[0].x += 0.5f;
	pVerts[0].y += 0.5f;
	pVerts[1].x += 0.5f;
	pVerts[1].y += 0.5f;
	pVerts[2].x += 0.5f;
	pVerts[2].y += 0.5f;
	pVerts[3].x += 0.5f;
	pVerts[3].y += 0.5f;

	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 0;
	pIndex[4] = 2;
	pIndex[5] = 3;

	pushcommand((f2dTextureInternal*)pTex, 4, 6);

	return FCYERR_OK;
}

fResult f2dGraphics2DImpl::DrawRaw(f2dTexture* pTex, fuInt VertCount, fuInt IndexCount, const f2dGraphics2DVertex* VertArr, const fuShort* IndexArr)
{
	if(!m_RenderStart)
		return FCYERR_ILLEGAL;

	f2dGraphics2DVertex* pVerts;
	fuShort* pIndex;
	if(!alloc(VertCount, IndexCount, &pVerts, &pIndex))
		return FCYERR_INVAILDPARAM;

	memcpy(pVerts, VertArr, sizeof(f2dGraphics2DVertex) * VertCount);
	memcpy(pIndex, IndexArr, sizeof(fuShort) * IndexCount);

	pushcommand((f2dTextureInternal*)pTex, VertCount, IndexCount);

	return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dPostEffectImpl::f2dPostEffectImpl(f2dRenderDeviceImpl* pDev, ID3DXEffect* pEffect)
	: m_pDev(pDev), m_pEffect(pEffect), m_StartEffect(false)
{
	if(FCYFAILED(m_pDev->CreateRenderTarget(m_pDev->GetBufferWidth(), m_pDev->GetBufferHeight(), (f2dTexture**)&m_pRT)))
		throw fcyException("f2dPostEffectImpl::f2dPostEffectImpl", "CreateRenderTarget Failed.");

	preInitEffect();

	m_pDev->AttachListener(this);
}

f2dPostEffectImpl::~f2dPostEffectImpl()
{
	m_pDev->RemoveListener(this);

	// 清空额外渲染目标
	for(fuInt i = 0; i<m_AdditionRT.size(); i++)
	{
		m_AdditionRT[i]->Release();
	}
	m_AdditionRT.clear();

	// 清空所有渲染表面
	for(fuInt i = 0; i<m_PassTarget.size(); i++)
	{
		FCYSAFEKILL(m_PassTarget[i]);
	}
	m_PassTarget.clear();

	FCYSAFEKILL(m_pRT);
	FCYSAFEKILL(m_pEffect);
}

void f2dPostEffectImpl::preInitEffect()
{
	D3DXEFFECT_DESC tDesc;

	// 获得特效信息
	m_pEffect->GetDesc(&tDesc);
	
	// 遍历所有参数
	for(fuInt i = 0; i<tDesc.Parameters; i++)
	{
		D3DXHANDLE tParam = m_pEffect->GetParameter(NULL, i);

		// 检查语义
		D3DXHANDLE tObj = m_pEffect->GetAnnotationByName(tParam, "Type");
		if(tObj)
		{
			D3DXPARAMETER_DESC tParamDesc;

			// 提出名称
			m_pEffect->GetParameterDesc(tObj, &tParamDesc);

			if(tParamDesc.Type == D3DXPT_STRING)
			{
				fcStr tStr = NULL;
				m_pEffect->GetString(tObj, &tStr);
				
				// 绑定缓冲
				if(strcmp(tStr, "Src")==0)
				{
					m_pEffect->SetTexture(tParam, m_pRT->GetHandle());
				}
				else if(strcmp(tStr, "RenderTarget")==0)
				{
					// 创建一个纹理
					f2dTextureRenderTarget* pTarget = NULL;
					m_pDev->CreateRenderTarget(m_pDev->GetBufferWidth(), m_pDev->GetBufferHeight(), (f2dTexture**)&pTarget);
					if(pTarget)
					{
						m_pEffect->SetTexture(tParam, pTarget->GetHandle());
						m_AdditionRT.push_back(pTarget);
					}
				}
				else if(strcmp(tStr, "BackBufferWidth")==0)
				{
					// 设置值
					m_pEffect->SetFloat(tParam, (float)m_pDev->GetBufferWidth());
				}
				else if(strcmp(tStr, "BackBufferHeight")==0)
				{
					// 设置值
					m_pEffect->SetFloat(tParam, (float)m_pDev->GetBufferHeight());
				}
			}
		}
	}

	// 获得第一个technique并遍历所有的pass
	D3DXHANDLE hTech = m_pEffect->GetTechnique(0);
	D3DXTECHNIQUE_DESC tTechDesc;
	m_pEffect->GetTechniqueDesc(hTech, &tTechDesc);
	
	m_PassTarget.resize(tTechDesc.Passes);
	for(fuInt i = 0; i<tTechDesc.Passes; i++)
	{
		m_PassTarget[i] = NULL;

		D3DXHANDLE hPass = m_pEffect->GetPass(hTech, i);

		// 获得Pass的Target字段
		D3DXHANDLE hTarget = m_pEffect->GetAnnotationByName(hPass, "Target");
		if(hTarget)
		{
			D3DXPARAMETER_DESC tParamDesc;
			m_pEffect->GetParameterDesc(hTarget, &tParamDesc);

			if(tParamDesc.Type == D3DXPT_STRING)
			{
				fcStr tStr = NULL;
				m_pEffect->GetString(hTarget, &tStr);
				
				// 获得渲染目标
				D3DXHANDLE hTargetTex = m_pEffect->GetParameterByName(NULL, tStr);
				if(hTargetTex)
				{
					IDirect3DTexture9* pTex = NULL;
					if(FAILED(m_pEffect->GetTexture(hTargetTex, (IDirect3DBaseTexture9**)&pTex)))
						continue;

					// 记录到表
					IDirect3DSurface9* pSurface = NULL;
					pTex->GetSurfaceLevel(0, &pSurface);
					m_PassTarget[i] = pSurface;

					pTex->Release();
				}
			}
		}
	}

	m_pEffect->SetTechnique(hTech);
}

void f2dPostEffectImpl::OnRenderDeviceLost()
{
	m_pEffect->OnLostDevice();
	FCYSAFEKILL(m_pRT);

	// 清空渲染目标
	for(fuInt i = 0; i<m_AdditionRT.size(); i++)
	{
		m_AdditionRT[i]->Release();
	}
	m_AdditionRT.clear();

	// 清空所有渲染表面
	for(fuInt i = 0; i<m_PassTarget.size(); i++)
	{
		FCYSAFEKILL(m_PassTarget[i]);
	}
	m_PassTarget.clear();
}

void f2dPostEffectImpl::OnRenderDeviceReset()
{
	// 恢复渲染目标
	m_pDev->CreateRenderTarget(m_pDev->GetBufferWidth(), m_pDev->GetBufferHeight(), (f2dTexture**)&m_pRT);

	m_pEffect->OnResetDevice();

	// 重新获得参数
	preInitEffect();
}

fFloat f2dPostEffectImpl::GetFloat(fcStr ParamName)
{
	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return 0.f;
	
	FLOAT tValue = 0.f;
	if(FAILED(m_pEffect->GetFloat(hParam, &tValue)))
		return 0.f;
	else
		return tValue;
}

fResult f2dPostEffectImpl::GetFloatArr(fcStr ParamName, fFloat* Dest, fuInt Count)
{
	if(!Dest)
		return FCYERR_INVAILDPARAM;

	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return FCYERR_OBJNOTEXSIT;
	
	if(FAILED(m_pEffect->GetFloatArray(hParam, Dest, Count)))
		return FCYERR_ILLEGAL;
	else
		return FCYERR_OK;;
}

fResult f2dPostEffectImpl::SetValue(fcStr ParamName, fFloat Value)
{
	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return FCYERR_OBJNOTEXSIT;

	if(FAILED(m_pEffect->SetFloat(hParam, Value)))
		return FCYERR_ILLEGAL;
	else
		return FCYERR_OK;
}

fResult f2dPostEffectImpl::SetValue(fcStr ParamName, fuInt Index, fFloat Value)
{
	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return FCYERR_OBJNOTEXSIT;

	D3DXHANDLE hParamElement = m_pEffect->GetParameterElement(hParam, Index);
	if(!hParamElement)
		return FCYERR_OBJNOTEXSIT;

	if(FAILED(m_pEffect->SetFloat(hParamElement, Value)))
		return FCYERR_ILLEGAL;
	else
		return FCYERR_OK;
}

fResult f2dPostEffectImpl::SetFloatArr(fcStr ParamName, const fFloat* Src, fuInt Count)
{
	if(!Src)
		return FCYERR_INVAILDPARAM;

	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return FCYERR_OBJNOTEXSIT;

	if(FAILED(m_pEffect->SetFloatArray(hParam, Src, Count)))
		return FCYERR_ILLEGAL;
	else
		return FCYERR_OK;
}

fResult f2dPostEffectImpl::SetTexture(fcStr ParamName, f2dTexture* pTex)
{
	if(!pTex)
		return FCYERR_INVAILDPARAM;

	D3DXHANDLE hParam = m_pEffect->GetParameterByName(NULL, ParamName);
	if(!hParam)
		return FCYERR_OBJNOTEXSIT;

	if(FAILED(m_pEffect->SetTexture(hParam, ((f2dTextureInternal*)pTex)->GetHandle())))
		return FCYERR_ILLEGAL;
	else
		return FCYERR_OK;
}

fResult f2dPostEffectImpl::Begin()
{
	if(m_StartEffect)
		return FCYERR_ILLEGAL;
	m_StartEffect = true;

	m_pDev->PushRenderTarget(m_pRT);
	return FCYERR_OK;
}

fResult f2dPostEffectImpl::End()
{
	if(!m_StartEffect)
		return FCYERR_ILLEGAL;
	m_StartEffect = false;

	m_pDev->PopRenderTarget(); // 提出渲染目标

	IDirect3DDevice9* pDev = (IDirect3DDevice9*)m_pDev->GetHandle();
	fuInt tFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	float tVertArr[4][5] = 
	{
		{-1.001f,  1.001f, 0.f, 0.f, 0.f},
		{ 0.999f,  1.001f, 0.f, 1.f, 0.f},
		{-1.001f, -0.999f, 0.f, 0.f, 1.f},
		{ 0.999f, -0.999f, 0.f, 1.f, 1.f}
	};

	// 设置格式
	pDev->SetFVF(tFVF);

	// 绘制效果
	fuInt tNumPassed;
	m_pEffect->Begin( &tNumPassed, 0 );
	for(fuInt iPass = 0; iPass < tNumPassed; iPass++)
	{
		IDirect3DSurface9* pBack = NULL;

		if(m_PassTarget[iPass])
		{
			pDev->GetRenderTarget(0, &pBack);
			pDev->SetRenderTarget(0, m_PassTarget[iPass]);
		}

		m_pEffect->BeginPass( iPass );

		// 绘制
		pDev->BeginScene();

		pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tVertArr, sizeof(float)*5);

		pDev->EndScene();

		m_pEffect->EndPass();

		if(pBack)
		{
			pDev->SetRenderTarget(0, pBack);
			pBack->Release();
		}
	}
	m_pEffect->End();

	return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dRenderDeviceImpl::f2dRenderDeviceImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
	: m_pEngine(pEngine), m_pD3D9(NULL), m_pDev(NULL), m_hWnd((HWND)pEngine->GetMainWindow()->GetHandle()), m_pWinSurface(NULL),
	m_pBackBuffer(NULL), m_ListenerList(NULL),
	m_bDevLost(false)
{
	ZeroMemory(&m_D3Dpp,sizeof(m_D3Dpp));
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = BackBufferWidth;
	m_ScissorRect.bottom = BackBufferHeight;

	HRESULT tHR;

	// --- 创建D3D9 ---
	m_pD3D9 = m_API.DLLEntry_Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_pD3D9)
		throw fcyException("f2dRenderDevice::f2dRenderDevice", "Direct3DCreate9 Failed.");

	// --- 创建设备 ---
	// 填充属性
	m_D3Dpp.BackBufferCount = 1;                      // 后台缓冲页面个数
	m_D3Dpp.BackBufferWidth = BackBufferWidth;        // 缓冲页面宽度
	m_D3Dpp.BackBufferHeight = BackBufferHeight;      // 缓冲页面高度
	m_D3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;       // 页面格式
	m_D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;    // 模板和Z缓冲格式
	m_D3Dpp.EnableAutoDepthStencil = true;            // 开启自动创建模板和Z缓冲
	m_D3Dpp.Windowed = Windowed;                      // 是否窗口
	m_D3Dpp.PresentationInterval = VSync? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;	// 立即交换
	m_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if(AALevel != F2DAALEVEL_NONE && CheckMultiSample(AALevel, Windowed))
	{
		// 开启抗锯齿
		m_D3Dpp.MultiSampleQuality = 0;

		// 开启抗锯齿
		switch(AALevel)
		{
		case F2DAALEVEL_2:
			m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			break;
		case F2DAALEVEL_4:
			m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
			break;
		case F2DAALEVEL_8:
			m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
			break;
		case F2DAALEVEL_16:
			m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;
			break;
		default:
			m_D3Dpp.MultiSampleQuality = 0;
			break;
		}
	}

	tHR = m_pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_D3Dpp,
		&m_pDev
		);

	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pD3D9);
		throw fcyWin32COMException("f2dRenderDevice::f2dRenderDevice", "CreateDevice Failed.", tHR);
	}

	// --- 初始化渲染状态 ---
	initState();

	// --- 获取设备参数 ---
	D3DADAPTER_IDENTIFIER9 tIdentify;

	m_pD3D9->GetAdapterIdentifier(0, 0, &tIdentify);

	m_DevName = fcyStringHelper::MultiByteToWideChar(tIdentify.Description);
}

f2dRenderDeviceImpl::~f2dRenderDeviceImpl()
{
	// 释放对象
	clearRenderTarget();

	// 释放DX组件
	FCYSAFEKILL(m_pWinSurface);
	FCYSAFEKILL(m_pDev);
	FCYSAFEKILL(m_pD3D9);

	// 删除渲染器监听链
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		ListenerNode* p = pListener;

		pListener = pListener->pNext;

		delete p;
	}
}

void f2dRenderDeviceImpl::initState()
{
	// --- 初始化视口 ---
	m_ViewPort.Width = GetBufferWidth();
	m_ViewPort.Height = GetBufferHeight();
	m_ViewPort.MaxZ = 1.0f;
	m_ViewPort.MinZ = 0.01f;
	m_ViewPort.X = 0;
	m_ViewPort.Y = 0;

	m_pDev->SetViewport(&m_ViewPort);

	// --- 设置默认渲染状态 ---
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 关闭反面剔除
	m_pDev->SetRenderState(D3DRS_LIGHTING, FALSE);        // 关闭光照
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);          // 启动Z缓冲
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // 启动Alpha混合

	// --- 设置ZBUFFER ---
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	// --- 设置默认混合状态 ---
	m_pDev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
	m_pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// --- 纹理混合参数默认值 ---
	m_pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_ADD);    
	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	// --- 纹理寻址 ---
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// --- 设置采样器 ---
	// 三线性
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// --- 其他功能 ---
	// 打开矩形裁剪功能
	m_pDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	// 设置矩形裁剪
	m_pDev->SetScissorRect(&m_ScissorRect);
}

void f2dRenderDeviceImpl::sendDevLostMsg()
{
	clearRenderTarget();

	// 发送丢失消息
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		pListener->pListener->OnRenderDeviceLost();
		pListener = pListener->pNext;
	}
}

void f2dRenderDeviceImpl::sendDevResetMsg()
{
	// 发送重置消息
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		pListener->pListener->OnRenderDeviceReset();
		pListener = pListener->pNext;
	}
}

void f2dRenderDeviceImpl::clearRenderTarget()
{
	if(!m_RTList.empty())
	{
		while(!m_RTList.empty())
		{
			m_RTList.top()->Release();
			m_RTList.pop();
		}

		m_pDev->SetRenderTarget(0, m_pBackBuffer);
	}
	FCYSAFEKILL(m_pBackBuffer);
}

void* f2dRenderDeviceImpl::GetHandle()
{
	return m_pDev;
}

fResult f2dRenderDeviceImpl::SyncDevice()
{
	HRESULT tHR = m_pDev->TestCooperativeLevel();	// 设备协作测试
	
	if(m_bDevLost) // 设备已经丢失
	{	
		if(tHR == D3DERR_DEVICENOTRESET) // 尚未重置
		{
			tHR = m_pDev->Reset(&m_D3Dpp);

			if(SUCCEEDED(tHR)) // 重置成功
			{
				m_bDevLost = false;

				initState(); // 重新初始化状态

				sendDevResetMsg(); // 通知非托管组件恢复工作
			}
		}
	}
	else if(tHR==D3DERR_DEVICELOST || tHR==D3DERR_DEVICENOTRESET) 	// 设备尚未丢失，检查是否丢失
	{
		m_bDevLost = true; // 标记设备丢失

		sendDevLostMsg();
	}

	return (m_bDevLost ? FCYERR_INTERNALERR : FCYERR_OK);
}

fResult f2dRenderDeviceImpl::Present()
{
	clearRenderTarget();

	m_pDev->Present(0,0,0,0);

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::AttachListener(f2dRenderDeviceEventListener* Listener)
{
	if(Listener == NULL)
		return FCYERR_INVAILDPARAM;

	ListenerNode* pNew = new ListenerNode();
	pNew->pListener = Listener;
	pNew->pNext = m_ListenerList;
	m_ListenerList = pNew;

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::RemoveListener(f2dRenderDeviceEventListener* Listener)
{
	if(m_ListenerList)
	{
		if(m_ListenerList->pListener == Listener)
		{
			ListenerNode* p = m_ListenerList->pNext;
			delete m_ListenerList;
			m_ListenerList = p;
		}
		else
		{
			ListenerNode* p = m_ListenerList;
			while(p)
			{
				ListenerNode* pNext = p->pNext;

				if(pNext && pNext->pListener == Listener)
				{
					p->pNext = pNext->pNext;
					delete pNext;
					return FCYERR_OK;
				}

				p = pNext;
			}
		}
	}

	return FCYERR_OBJNOTEXSIT;
}

fResult f2dRenderDeviceImpl::CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	IDirect3DTexture9* pTex = NULL;
	HRESULT tHR = m_pDev->CreateTexture(Width, Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL);
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::CreateDynamicTexture", "CreateTexture Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	f2dTextureDynamic* pRet = new f2dTextureDynamic(this, pTex, Width, Height);
	
	*pOut = pRet;

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pStream)
		return FCYERR_INVAILDPARAM;

	// 读取整个流
	fByte* tData = new fByte[(size_t)pStream->GetLength()];
	if(FCYFAILED(pStream->SetPosition(FCYSEEKORIGIN_BEG, 0)))
	{
		FCYSAFEDELARR(tData);
		return FCYERR_INTERNALERR;
	}
	if(FCYFAILED(pStream->ReadBytes(tData, pStream->GetLength(), NULL)))
	{
		FCYSAFEDELARR(tData);
		return FCYERR_INTERNALERR;
	}

	// 创建Texture
	D3DXIMAGE_INFO tInfo;
	IDirect3DTexture9* pTex = NULL;
	HRESULT tHR = m_API.DLLEntry_D3DXCreateTextureFromFileInMemoryEx(
		m_pDev,
		tData,
		(fuInt)pStream->GetLength(),
		Width == 0 ? D3DX_DEFAULT_NONPOW2 : Width,
		Height == 0 ? D3DX_DEFAULT_NONPOW2 : Height,
		(!HasMipmap || IsDynamic) ? 1 : D3DX_DEFAULT,
		IsDynamic ? D3DUSAGE_DYNAMIC : 0,
		D3DFMT_A8R8G8B8,
		IsDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
		D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
		0,
		&tInfo,
		NULL,
		&pTex
	);

	FCYSAFEDELARR(tData);
	
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::CreateTextureFromStream", "D3DXCreateTextureFromFileInMemoryEx Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	if(IsDynamic)
	{
		f2dTextureDynamic* pRet = new f2dTextureDynamic(
			this, 
			pTex, 
			Width == 0 ? tInfo.Width : Width, 
			Height == 0 ? tInfo.Height : Height
			);

		*pOut = pRet;
	}
	else
	{
		f2dTextureStatic* pRet = new f2dTextureStatic(
			pTex,
			Width == 0 ? tInfo.Width : Width, 
			Height == 0 ? tInfo.Height : Height
			);

		*pOut = pRet;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateRenderTarget(fuInt Width, fuInt Height, f2dTexture** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	IDirect3DTexture9* pTex = NULL;
	HRESULT tHR = m_pDev->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL);
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::CreateRenderTarget", "CreateTexture Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	f2dTextureRenderTarget* pRet = new f2dTextureRenderTarget(this, pTex, Width, Height);
	
	*pOut = pRet;

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(VertexBufferSize == 0)
		VertexBufferSize = 512;
	if(IndexBufferSize == 0)
		IndexBufferSize = 768;

	try
	{
		*pOut = new f2dGraphics2DImpl(this, VertexBufferSize, IndexBufferSize);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreatePostEffect(f2dStream* pStream, f2dPostEffect** pOut, f2dStream** pErrOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pStream)
		return FCYERR_INVAILDPARAM;

	// 读取所有代码
	fByte *tCodes = new fByte[(size_t)pStream->GetLength()];
	pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);
	if(FCYFAILED(pStream->ReadBytes(tCodes, pStream->GetLength(), NULL)))
	{
		FCYSAFEDELARR(tCodes);
		return FCYERR_INTERNALERR;
	}

	// 创建效果
	ID3DXEffect* pEffect = NULL;
	ID3DXBuffer* pErr = NULL;
	HRESULT tHR = m_API.DLLEntry_D3DXCreateEffectEx(m_pDev, tCodes, (UINT)pStream->GetLength(), NULL, NULL, NULL, 0, NULL, &pEffect, &pErr);
	FCYSAFEDELARR(tCodes);

	// 失败处理
	if(FAILED(tHR))
	{
		if(pErrOut)
		{
			*pErrOut = NULL;
			if(pErr)
				*pErrOut = new fcyMemStream((fcData)pErr->GetBufferPointer(), pErr->GetBufferSize(), false, false);
		}

		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::CreatePostEffect", "D3DXCreateEffectEx Failed.", tHR));

		return FCYERR_INTERNALERR;
	}
	
	// 创建效果
	try
	{
		*pOut = new f2dPostEffectImpl(this, pEffect);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		FCYSAFEKILL(pEffect);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::Clear(const fcyColor& BackBufferColor, fFloat ZValue)
{
	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, BackBufferColor.argb, ZValue, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::ClearColor(const fcyColor& BackBufferColor)
{
	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, BackBufferColor.argb, 1.f, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::ClearZBuffer(fFloat Value)
{
	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, Value, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

f2dTexture* f2dRenderDeviceImpl::GetRenderTarget()
{
	if(m_RTList.empty())
		return NULL;
	else
		return m_RTList.top();
}

fResult f2dRenderDeviceImpl::PushRenderTarget(f2dTexture* pTarget)
{
	if(!pTarget->IsRenderTarget())
		return FCYERR_INVAILDPARAM;
	else
	{
		if(m_RTList.empty())
		{
			m_pDev->GetRenderTarget(0, &m_pBackBuffer);
		}

		m_pDev->SetRenderTarget(0, ((f2dTextureRenderTarget*)pTarget)->GetSurface());
		m_RTList.push((f2dTextureRenderTarget*)pTarget);
		pTarget->AddRef();
		
		return FCYERR_OK;
	}
}

fResult f2dRenderDeviceImpl::PopRenderTarget()
{
	if(m_RTList.empty())
		return FCYERR_ILLEGAL;
	else
	{
		m_RTList.top()->Release();
		m_RTList.pop();
		
		if(m_RTList.empty())
		{
			m_pDev->SetRenderTarget(0, m_pBackBuffer);
			FCYSAFEKILL(m_pBackBuffer);
		}
		else
		{
			m_pDev->SetRenderTarget(0, m_RTList.top()->GetSurface());
		}

		return FCYERR_OK;
	}
}

fcyRect f2dRenderDeviceImpl::GetScissorRect()
{
	fcyRect tRet((float)m_ScissorRect.left, (float)m_ScissorRect.top, (float)m_ScissorRect.right, (float)m_ScissorRect.bottom);
	return tRet;
}

fResult f2dRenderDeviceImpl::SetScissorRect(const fcyRect& pRect)
{
	m_ScissorRect.left = (int)pRect.a.x;
	m_ScissorRect.top = (int)pRect.a.y;
	m_ScissorRect.right = (int)pRect.b.x;
	m_ScissorRect.bottom = (int)pRect.b.y;

	if(FAILED(m_pDev->SetScissorRect(&m_ScissorRect)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fcStrW f2dRenderDeviceImpl::GetDeviceName()
{
	return m_DevName.c_str();
}

fuInt f2dRenderDeviceImpl::GetBufferWidth()
{
	return m_D3Dpp.BackBufferWidth;
}

fuInt f2dRenderDeviceImpl::GetBufferHeight()
{
	return m_D3Dpp.BackBufferHeight;
}

fBool f2dRenderDeviceImpl::IsWindowed()
{
	return m_D3Dpp.Windowed == 0 ? false : true;
}

F2DAALEVEL f2dRenderDeviceImpl::GetAALevel()
{
	switch(m_D3Dpp.MultiSampleType)
	{
	case D3DMULTISAMPLE_2_SAMPLES:
		return F2DAALEVEL_2;
	case D3DMULTISAMPLE_4_SAMPLES:
		return F2DAALEVEL_4;
	case D3DMULTISAMPLE_8_SAMPLES:
		return F2DAALEVEL_8;
	case D3DMULTISAMPLE_16_SAMPLES:
		return F2DAALEVEL_16;
	}
	return F2DAALEVEL_NONE;
}

fBool f2dRenderDeviceImpl::CheckMultiSample(F2DAALEVEL AALevel, fBool Windowed)
{
	DWORD Quality = 0;
	HRESULT tHr;

	switch(AALevel)
	{
	case F2DAALEVEL_NONE:
		return true;
	case F2DAALEVEL_2:
		tHr = m_pD3D9->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, Windowed, D3DMULTISAMPLE_2_SAMPLES, &Quality);
		break;
	case F2DAALEVEL_4:
		tHr = m_pD3D9->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, Windowed, D3DMULTISAMPLE_4_SAMPLES, &Quality);
		break;
	case F2DAALEVEL_8:
		tHr = m_pD3D9->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, Windowed, D3DMULTISAMPLE_8_SAMPLES, &Quality);
		break;
	case F2DAALEVEL_16:
		tHr = m_pD3D9->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, Windowed, D3DMULTISAMPLE_16_SAMPLES, &Quality);
		break;
	default:
		return false;
	}

	if(FAILED(tHr))
		return false;
	else
		return true;
}

fResult f2dRenderDeviceImpl::SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
{
	// 准备参数
	D3DPRESENT_PARAMETERS tD3Dpp;
	ZeroMemory(&tD3Dpp, sizeof(D3DPRESENT_PARAMETERS));

	tD3Dpp.BackBufferCount = 1;                      // 后台缓冲页面个数
	tD3Dpp.BackBufferWidth = Width;	                 // 缓冲页面宽度
	tD3Dpp.BackBufferHeight = Height;                // 缓冲页面高度
	tD3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;       // 页面格式
	tD3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;    // 模板和Z缓冲格式
	tD3Dpp.EnableAutoDepthStencil = true;            // 开启自动创建模板和Z缓冲
	tD3Dpp.Windowed = Windowed;                      // 是否窗口
	tD3Dpp.PresentationInterval = VSync? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;	// 立即交换
	tD3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	tD3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if(AALevel != F2DAALEVEL_NONE)
	{
		tD3Dpp.MultiSampleQuality = 0;

		// 开启抗锯齿
		switch(AALevel)
		{
		case F2DAALEVEL_2:
			tD3Dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			break;
		case F2DAALEVEL_4:
			tD3Dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
			break;
		case F2DAALEVEL_8:
			tD3Dpp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
			break;
		case F2DAALEVEL_16:
			tD3Dpp.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;
			break;
		default:
			tD3Dpp.MultiSampleQuality = 0;
			break;
		}
	}

	// 销毁所有未托管对象
	m_bDevLost = true;
	sendDevLostMsg();

	// 重设渲染系统
	HRESULT tHR = m_pDev->Reset(&tD3Dpp);
	if(FAILED(tHR))
	{
		// 设置失败，保留为丢失状态，等待循环时恢复上次状态。
		return FCYERR_INVAILDPARAM;
	}
	else
	{
		// 成功，恢复所有状态

		m_bDevLost = false;
		m_D3Dpp = tD3Dpp;

		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = Width;
		m_ScissorRect.bottom = Height;

		initState();

		sendDevResetMsg();

		return FCYERR_OK;
	}
}

fResult f2dRenderDeviceImpl::SaveScreen(f2dStream* pStream)
{
	if(m_bDevLost)
		return FCYERR_ILLEGAL;

	if(!pStream || !pStream->CanWrite() || !pStream->CanResize())
		return FCYERR_INVAILDPARAM;

	IDirect3DSurface9* pSurface = NULL;

	HRESULT tHR = m_pDev->CreateOffscreenPlainSurface(GetBufferWidth(), GetBufferHeight(),
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSurface, NULL);

	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::SaveScreen", "CreateOffscreenPlainSurface Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	if(FAILED(m_pDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface)))
	{
		FCYSAFEKILL(pSurface);
		return FCYERR_INTERNALERR;
	}

	ID3DXBuffer* pDataBuffer = NULL;
	tHR = m_API.DLLEntry_D3DXSaveSurfaceToFileInMemory(&pDataBuffer, D3DXIFF_PNG, pSurface, NULL, NULL);
	FCYSAFEKILL(pSurface);
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::SaveScreen", "D3DXSaveSurfaceToFileInMemory Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	// 保存到流
	if(FCYFAILED(pStream->WriteBytes((fcData)pDataBuffer->GetBufferPointer(), pDataBuffer->GetBufferSize(), NULL)))
	{
		FCYSAFEKILL(pDataBuffer);
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pDataBuffer);

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SaveTexture(f2dStream* pStream, f2dTexture* pTex)
{
	if(m_bDevLost)
		return FCYERR_ILLEGAL;

	if(!pStream || !pTex || !pStream->CanWrite() || !pStream->CanResize())
		return FCYERR_INVAILDPARAM;

	IDirect3DSurface9* pSurface = NULL;
	if(pTex->IsDynamic())
		((IDirect3DTexture9*)((f2dTextureDynamic*)pTex)->GetHandle())->GetSurfaceLevel(0, &pSurface);
	else if(pTex->IsRenderTarget())
	{
		pSurface = ((f2dTextureRenderTarget*)pTex)->GetSurface();
		pSurface->AddRef();
	}
	else
		((IDirect3DTexture9*)((f2dTextureStatic*)pTex)->GetHandle())->GetSurfaceLevel(0, &pSurface);

	if(!pSurface)
		return FCYERR_INTERNALERR;

	ID3DXBuffer* pDataBuffer = NULL;
	HRESULT tHR = m_API.DLLEntry_D3DXSaveSurfaceToFileInMemory(&pDataBuffer, D3DXIFF_PNG, pSurface, NULL, NULL);
	FCYSAFEKILL(pSurface);
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyWin32COMException("f2dRenderDeviceImpl::SaveTexture", "D3DXSaveSurfaceToFileInMemory Failed.", tHR));
		return FCYERR_INTERNALERR;
	}

	// 保存到流
	if(FCYFAILED(pStream->WriteBytes((fcData)pDataBuffer->GetBufferPointer(), pDataBuffer->GetBufferSize(), NULL)))
	{
		FCYSAFEKILL(pDataBuffer);
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pDataBuffer);

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::UpdateScreenToWindow(fcyColor KeyColor, fByte Alpha)
{
	fuInt tBackWidth = GetBufferWidth();
	fuInt tBackHeight = GetBufferHeight();

	// === 检查窗口大小是否匹配 ===
	if(m_DC.GetSafeHdc() == NULL || m_pWinSurface == NULL || (fuInt)m_DC.GetWidth() != tBackWidth || (fuInt)m_DC.GetHeight() != tBackHeight)
	{
		m_DC.Delete();
		m_DC.Create(tBackWidth, tBackHeight);

		// 创建离屏页面
		FCYSAFEKILL(m_pWinSurface);
		if(FAILED(m_pDev->CreateOffscreenPlainSurface(tBackWidth, tBackHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pWinSurface, NULL)))
			return FCYERR_INTERNALERR;
	}

	// === 准备参数 ===
	RECT tWinRect;
	::GetWindowRect(m_hWnd, &tWinRect);
	HDC tWinDC = GetWindowDC(m_hWnd);
	// 窗口位置
	POINT ptWinPos = { tWinRect.left, tWinRect.top };
	// DC数据起始位置
	POINT ptSrc = { 0, 0 };
	// 窗口大小
	SIZE szWin = { tWinRect.right - tWinRect.left, tWinRect.bottom - tWinRect.top };
	// DC表面大小
	RECT tDCSurface = { 0, 0, tBackWidth, tBackHeight };
	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, Alpha, AC_SRC_ALPHA };
	
	// === 拷贝BackSurface ===
	D3DLOCKED_RECT tD3DLockRect;
	IDirect3DSurface9* pOrgSurface = NULL;

	if(FAILED(m_pDev->GetRenderTarget(0, &pOrgSurface)))
		return FCYERR_INTERNALERR;
	if(FAILED(m_pDev->GetRenderTargetData(pOrgSurface, m_pWinSurface)))
	{
		FCYSAFEKILL(pOrgSurface);
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pOrgSurface);

	// === 拷贝到GDI ===
	if(FAILED(m_pWinSurface->LockRect(&tD3DLockRect, &tDCSurface, D3DLOCK_READONLY)))
		return FCYERR_INTERNALERR;
	fuInt y = 0;
	fuInt x = 0;
	fcyColor* tDest = (fcyColor*)m_DC.GetBits();
	fcyColor* tSrc = (fcyColor*)tD3DLockRect.pBits;
	while(y < tBackHeight)
	{
		*tDest = tSrc[x];
		tDest++; x++;
		if(x >= tBackWidth)
		{
			tSrc = (fcyColor*)((fByte*)tSrc + tD3DLockRect.Pitch);
			x = 0;
			y++;
		}
	}
	m_pWinSurface->UnlockRect();

	if(FALSE == UpdateLayeredWindow(m_hWnd, tWinDC, &ptWinPos, &szWin, m_DC.GetSafeHdc(), &ptSrc, 0, &stBlend, ULW_ALPHA))
		return FCYERR_INTERNALERR;
	
	ReleaseDC(m_hWnd, tWinDC);

	return FCYERR_OK;
}
