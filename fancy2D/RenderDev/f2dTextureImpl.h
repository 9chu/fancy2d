////////////////////////////////////////////////////////////////////////////////
/// @file  f2dTextureImpl.h
/// @brief fancy2D �����װ
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dRenderDeviceAPI.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��̬��ά����
////////////////////////////////////////////////////////////////////////////////
class f2dTexture2DStatic :
	public fcyRefObjImpl<f2dTexture2D>
{
	friend class f2dRenderDeviceImpl;
private:
	IDirect3DTexture9* m_pTex;
	fuInt m_Width;
	fuInt m_Height;
public: // �ӿ�ʵ��
	void* GetHandle() { return m_pTex; }
	fuInt GetDimension() { return 2; }
	fBool IsDynamic() { return false; }
	fBool IsRenderTarget() { return false; }

	fuInt GetWidth() { return m_Width; }
	fuInt GetHeight() { return m_Height; }

	fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut) { return FCYERR_NOTSUPPORT; }

	fResult Unlock() { return FCYERR_NOTSUPPORT; }
protected:
	f2dTexture2DStatic(f2dRenderDevice* pDev, f2dStream* pStream, fuInt Width, fuInt Height, fBool HasMipmap);
	f2dTexture2DStatic(f2dRenderDevice* pDev, fcData pMemory, fLen Size, fuInt Width, fuInt Height, fBool HasMipmap);
	~f2dTexture2DStatic();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��̬��ά����
////////////////////////////////////////////////////////////////////////////////
class f2dTexture2DDynamic :
	public fcyRefObjImpl<f2dTexture2D>,
	public f2dRenderDeviceEventListener
{
	friend class f2dRenderDeviceImpl;
private:
	f2dRenderDevice* m_pParent;

	IDirect3DTexture9* m_pTex;
	fuInt m_Width;
	fuInt m_Height;
public: // �豸��ʧ
	void OnRenderDeviceLost();
	void OnRenderDeviceReset();
public: // �ӿ�ʵ��
	void* GetHandle() { return m_pTex; }
	fuInt GetDimension() { return 2; }
	fBool IsDynamic() { return true; }
	fBool IsRenderTarget() { return false; }

	fuInt GetWidth() { return m_Width; }
	fuInt GetHeight() { return m_Height; }

	fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut);

	fResult Unlock();
protected:
	f2dTexture2DDynamic(f2dRenderDevice* pDev, fuInt Width, fuInt Height);
	f2dTexture2DDynamic(f2dRenderDevice* pDev, f2dStream* pStream, fuInt Width, fuInt Height);
	f2dTexture2DDynamic(f2dRenderDevice* pDev, fcData pMemory, fLen Size, fuInt Width, fuInt Height);
	~f2dTexture2DDynamic();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��ά��ȾĿ��
////////////////////////////////////////////////////////////////////////////////
class f2dTexture2DRenderTarget :
	public fcyRefObjImpl<f2dTexture2D>,
	public f2dRenderDeviceEventListener
{
	friend class f2dRenderDeviceImpl;
private:
	f2dRenderDevice* m_pParent;

	IDirect3DTexture9* m_pTex;
	fuInt m_Width;
	fuInt m_Height;

	fBool m_bAutoResize;

	IDirect3DSurface9* m_pSurface;
public: // �ڲ������ӿ�
	IDirect3DSurface9* GetSurface();
public: // �豸��ʧ
	void OnRenderDeviceLost();
	void OnRenderDeviceReset();
public: // �ӿ�ʵ��
	void* GetHandle() { return m_pTex; }
	fuInt GetDimension() { return 2; }
	fBool IsDynamic() { return false; }
	fBool IsRenderTarget() { return true; }

	fuInt GetWidth() { return m_Width; }
	fuInt GetHeight() { return m_Height; }

	fResult Lock(fcyRect* pLockRect, fBool Discard, fuInt* Pitch, fData* pOut) { return FCYERR_NOTSUPPORT; }

	fResult Unlock() { return FCYERR_NOTSUPPORT; }
protected:
	f2dTexture2DRenderTarget(f2dRenderDevice* pDev, fuInt Width, fuInt Height, fBool AutoResize);
	~f2dTexture2DRenderTarget();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D���ģ�建��
////////////////////////////////////////////////////////////////////////////////
class f2dDepthStencilSurfaceImpl :
	public fcyRefObjImpl<f2dDepthStencilSurface>,
	public f2dRenderDeviceEventListener
{
	friend class f2dRenderDeviceImpl;
private:
	f2dRenderDevice* m_pParent;
	
	IDirect3DSurface9* m_pSurface;
	fuInt m_Width;
	fuInt m_Height;

	fBool m_bAutoResize;
	fBool m_bDiscard;
public: // �豸��ʧ
	void OnRenderDeviceLost();
	void OnRenderDeviceReset();
public: // �ӿ�ʵ��
	void* GetHandle();
	fuInt GetWidth();
	fuInt GetHeight();
protected:
	f2dDepthStencilSurfaceImpl(f2dRenderDevice* pDev, fuInt Width, fuInt Height, fBool AutoResize, fBool Discard);
	~f2dDepthStencilSurfaceImpl();
};
