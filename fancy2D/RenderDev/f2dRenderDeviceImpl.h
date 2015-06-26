////////////////////////////////////////////////////////////////////////////////
/// @file  f2dRenderDeviceImpl.h
/// @brief fancy2D��Ⱦ�豸�ӿ�ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dRenderDeviceAPI.h"

#include "../Engine/f2dWindowImpl.h"
#include "../f2dEngine.h"

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��Ⱦ�豸ʵ��
////////////////////////////////////////////////////////////////////////////////
class f2dRenderDeviceImpl :
	public fcyRefObjImpl<f2dRenderDevice>
{
private:
	struct ListenerNode
	{
		f2dRenderDeviceEventListener* pListener;
		fInt Priority;   // ���ȼ�

		ListenerNode* pNext;
	};

	// ����ӷ����̵߳���TestCooperativeLevel������
	class DeviceSyncTest
		: public f2dMainThreadDelegate
	{
	private:
		IDirect3DDevice9* m_pDev;
		HRESULT m_HR;
	public:
		void AddRef() { /* do nothing */ }
		void Release() { /* do nothing */ }
		void Excute() { m_HR = m_pDev->TestCooperativeLevel(); }
		void Reset() { m_HR = S_OK; }
		HRESULT GetResult()const { return m_HR; }
	public:
		DeviceSyncTest(IDirect3DDevice9* p)
			: m_pDev(p), m_HR(S_OK) {}
	};

	struct VertexDeclareInfo
	{
		fuInt Hash;
		std::vector<f2dVertexElement> ElementData;
		IDirect3DVertexDeclaration9* pVertexDeclare;
		fuInt VertexSize;

		VertexDeclareInfo();
		VertexDeclareInfo(const VertexDeclareInfo& Org);
		~VertexDeclareInfo();
	protected:
		VertexDeclareInfo& operator=(const VertexDeclareInfo& Right);
	};
private:
	f2dEngineImpl* m_pEngine;
	DWORD m_CreateThreadID;

	// API���
	f2dRenderDeviceAPI m_API;

	// DirectX���
	IDirect3D9* m_pD3D9;
	IDirect3DDevice9* m_pDev;
	D3DPRESENT_PARAMETERS m_D3Dpp;
	D3DVIEWPORT9 m_ViewPort;
	std::string m_DevName;

	// ���߳��¼�
	DeviceSyncTest* m_pSyncTestObj;
	
	// ����������
	ListenerNode* m_ListenerList;

	// ��������
	std::vector<VertexDeclareInfo> m_VDCache;

	// �豸״̬
	bool m_bDevLost;                              // �豸��ʧ��־
	IDirect3DSurface9* m_pBackBuffer;             // ����ȾĿ��
	IDirect3DSurface9* m_pBackDepthBuffer;        // �����ģ�建��
	RECT m_ScissorRect;                           // �ü�����
	
	f2dTexture2D* m_pCurBackBuffer;               // ��¼��ǰ�ĺ󱸻�����
	f2dDepthStencilSurface* m_pCurBackDepthBuffer;// ��¼��ǰ�ĺ���Ȼ�����

	f2dGraphics* m_pCurGraphics;   // ��ǰ�Ļ�ͼ����ֻ��¼ָ��
	fcyMatrix4 m_CurWorldMat;      // ��ǰ�������
	fcyMatrix4 m_CurLookatMat;     // ��ǰ�۲����
	fcyMatrix4 m_CurProjMat;       // ��ǰͶӰ����
	f2dBlendState m_CurBlendState; // ��ǰ���״̬

	IDirect3DVertexDeclaration9* m_pCurVertDecl; // ��ǰ�Ķ�������
	D3DTEXTUREOP m_CurTexBlendOP_Color;          // ��ǰ�������������

	// Window
	HWND m_hWnd;
	f2dWindowDC m_DC;
	IDirect3DSurface9* m_pWinSurface;
private:
	HRESULT doReset(D3DPRESENT_PARAMETERS* pD3DPP);  // ��֤�����߳�ִ��
	HRESULT doTestCooperativeLevel();
	void initState();         // ��ʼ��״̬
	int sendDevLostMsg();     // �����豸��ʧ�¼�, ���ض�����Ŀ
	int sendDevResetMsg();    // �����豸�����¼�
public: // �ڲ�����
	f2dRenderDeviceAPI& GetAPI() { return m_API; }   // ����API����
	f2dEngineImpl* GetEngine() { return m_pEngine; } // �����������
	fResult SyncDevice();                           // Э�����ԣ�����豸��ʧ����
	fResult Present();                              // ����

	// �����ݽ�״̬
	f2dGraphics* QueryCurGraphics() { return m_pCurGraphics; }
	fResult SubmitCurGraphics(f2dGraphics* pGraph, bool bDirty);  // ͬ��Graphics״̬���豸
	fResult SubmitWorldMat(const fcyMatrix4& Mat);                // �����ύ����任����
	fResult SubmitLookatMat(const fcyMatrix4& Mat);               // �����ύ�۲����
	fResult SubmitProjMat(const fcyMatrix4& Mat);                 // �����ύͶӰ����
	fResult SubmitBlendState(const f2dBlendState& State);         // �����ύ���״̬

	fResult SubmitVD(IDirect3DVertexDeclaration9* pVD);        // �����ݽ���������
	fResult SubmitTextureBlendOP_Color(D3DTEXTUREOP ColorOP);  // �����ݽ�����������

	// ע�ᶥ������
	IDirect3DVertexDeclaration9* RegisterVertexDeclare(f2dVertexElement* pElement, fuInt ElementCount, fuInt& ElementSize);
public: // �ӿ�ʵ��
	void* GetHandle() { return m_pDev; }
	fcStr GetDeviceName() { return m_DevName.c_str(); }
	fuInt GetBufferWidth() { return m_D3Dpp.BackBufferWidth; }
	fuInt GetBufferHeight() { return m_D3Dpp.BackBufferHeight; }
	fBool IsWindowed() { return m_D3Dpp.Windowed == TRUE ? 1 : 0; }
	F2DAALEVEL GetAALevel();
	fBool CheckMultiSample(F2DAALEVEL AALevel, fBool Windowed);
	fuInt GetSupportResolutionCount();
	fcyVec2 EnumSupportResolution(fuInt Index);
	fResult SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel);

	fResult AttachListener(f2dRenderDeviceEventListener* Listener, fInt Priority=0);
	fResult RemoveListener(f2dRenderDeviceEventListener* Listener);

	fResult CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut);
	fResult CreateTextureFromMemory(fcData pMemory, fLen Size, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut);
	fResult CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture2D** pOut);
	fResult CreateRenderTarget(fuInt Width, fuInt Height, fBool AutoResize , f2dTexture2D** pOut);
	fResult CreateDepthStencilSurface(fuInt Width, fuInt Height, fBool Discard, fBool AutoResize, f2dDepthStencilSurface** pOut);
	fResult CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut);
	fResult CreateGraphics3D(f2dEffect* pDefaultEffect, f2dGraphics3D** pOut);
	fResult CreateEffect(f2dStream* pStream, fBool bAutoState, f2dEffect** pOut);
	fResult CreateMeshData(f2dVertexElement* pVertElement, fuInt ElementCount, fuInt VertCount, fuInt IndexCount, fBool Int32Index, f2dMeshData** pOut);

	fResult Clear(const fcyColor& BackBufferColor = 0, fFloat ZValue = 1.0f);
	fResult Clear(const fcyColor& BackBufferColor = 0, fFloat ZValue = 1.0f, fuInt StencilValue = 0);
	fResult ClearColor(const fcyColor& BackBufferColor = 0);
	fResult ClearZBuffer(fFloat Value=1.0f);
	fResult ClearStencilBuffer(fuInt StencilValue=0);
	f2dTexture2D* GetRenderTarget();
	fResult SetRenderTarget(f2dTexture2D* pTex);
	f2dDepthStencilSurface* GetDepthStencilSurface();
	fResult SetDepthStencilSurface(f2dDepthStencilSurface* pSurface);
	fcyRect GetScissorRect();
	fResult SetScissorRect(const fcyRect& pRect);

	fResult SaveScreen(f2dStream* pStream);
	fResult SaveTexture(f2dStream* pStream, f2dTexture2D* pTex);
	fResult UpdateScreenToWindow(fcyColor KeyColor, fByte Alpha);
public:
	f2dRenderDeviceImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel);
	~f2dRenderDeviceImpl();
};
