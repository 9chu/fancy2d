#include "f2dRenderDeviceImpl.h"

#include <fcyMisc/fcyStringHelper.h>
#include <fcyMisc/fcyHash.h>
#include <fcyOS/fcyDebug.h>

#include "f2dTextureImpl.h"
#include "f2dGraphics2DImpl.h"
#include "f2dGraphics3DImpl.h"
#include "f2dEffectImpl.h"
#include "f2dMeshDataImpl.h"

#include "../Engine/f2dEngineImpl.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

f2dRenderDeviceImpl::VertexDeclareInfo::VertexDeclareInfo()
	: Hash(0), pVertexDeclare(NULL)
{}

f2dRenderDeviceImpl::VertexDeclareInfo::VertexDeclareInfo(const VertexDeclareInfo& Org)
	: Hash(Org.Hash), ElementData(Org.ElementData), pVertexDeclare(Org.pVertexDeclare)
{
	if(pVertexDeclare)
		pVertexDeclare->AddRef();
}

f2dRenderDeviceImpl::VertexDeclareInfo::~VertexDeclareInfo()
{
	FCYSAFEKILL(pVertexDeclare);
}

////////////////////////////////////////////////////////////////////////////////

f2dRenderDeviceImpl::f2dRenderDeviceImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
: m_pEngine(pEngine), m_pD3D9(NULL), m_pDev(NULL), m_hWnd((HWND)pEngine->GetMainWindow()->GetHandle()), 
	m_pSyncTestObj(NULL), m_bDevLost(false), m_pBackBuffer(NULL), m_pBackDepthBuffer(NULL), m_pCurGraphics(NULL),
	m_ListenerList(NULL), m_pWinSurface(NULL), m_pCurBackBuffer(NULL), m_pCurBackDepthBuffer(NULL),
	m_pCurVertDecl(NULL), m_CreateThreadID(GetCurrentThreadId())
{
	ZeroMemory(&m_D3Dpp,sizeof(m_D3Dpp));
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = BackBufferWidth;
	m_ScissorRect.bottom = BackBufferHeight;

	HRESULT tHR;

	// --- ����D3D9 ---
	m_pD3D9 = m_API.DLLEntry_Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_pD3D9)
		throw fcyException("f2dRenderDeviceImpl::f2dRenderDeviceImpl", "Direct3DCreate9 Failed.");

	// --- �����豸 ---
	// �������
	m_D3Dpp.BackBufferCount = 2;                      // ��̨����ҳ�����
	m_D3Dpp.BackBufferWidth = BackBufferWidth;        // ����ҳ����
	m_D3Dpp.BackBufferHeight = BackBufferHeight;      // ����ҳ��߶�
	m_D3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;       // ҳ���ʽ
	m_D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;    // ģ���Z�����ʽ
	m_D3Dpp.EnableAutoDepthStencil = true;            // �����Զ�����ģ���Z����
	m_D3Dpp.Windowed = Windowed;                      // �Ƿ񴰿�
	m_D3Dpp.PresentationInterval = VSync? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;	// ��������
	m_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if(AALevel != F2DAALEVEL_NONE && CheckMultiSample(AALevel, Windowed))
	{
		// ���������
		m_D3Dpp.MultiSampleQuality = 0;

		// ���������
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
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, // �������߳�
		&m_D3Dpp,
		&m_pDev
		);

	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pD3D9);
		throw fcyWin32COMException("f2dRenderDeviceImpl::f2dRenderDeviceImpl", "IDirect3D9::CreateDevice Failed.", tHR);
	}
	else
		m_pSyncTestObj = new DeviceSyncTest(m_pDev);

	// --- ��ȡ�豸���� ---
	D3DADAPTER_IDENTIFIER9 tIdentify;
	m_pD3D9->GetAdapterIdentifier(0, 0, &tIdentify);
	m_DevName = tIdentify.Description;

	// --- ��ʼ����Ⱦ״̬ ---
	m_ViewPort.Width = GetBufferWidth();
	m_ViewPort.Height = GetBufferHeight();
	m_ViewPort.MaxZ = 1.0f;
	m_ViewPort.MinZ = 0.0f;
	m_ViewPort.X = 0;
	m_ViewPort.Y = 0;

	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = GetBufferWidth();
	m_ScissorRect.bottom = GetBufferHeight();
	
	m_CurBlendState.BlendOp = F2DBLENDOPERATOR_ADD;
	m_CurBlendState.SrcBlend = F2DBLENDFACTOR_SRCALPHA;
	m_CurBlendState.DestBlend = F2DBLENDFACTOR_INVSRCALPHA;
	m_CurBlendState.AlphaBlendOp = F2DBLENDOPERATOR_ADD;
	m_CurBlendState.AlphaSrcBlend = F2DBLENDFACTOR_SRCALPHA;
	m_CurBlendState.AlphaDestBlend = F2DBLENDFACTOR_INVSRCALPHA;

	m_CurTexBlendOP_Color = D3DTOP_ADD;

	m_CurWorldMat = fcyMatrix4::GetIdentity();
	m_CurLookatMat = fcyMatrix4::GetIdentity();
	m_CurProjMat = fcyMatrix4::GetIdentity();

	initState();
}

f2dRenderDeviceImpl::~f2dRenderDeviceImpl()
{
	// �ͷŶ���
	FCYSAFEKILL(m_pCurBackBuffer);
	FCYSAFEKILL(m_pCurBackDepthBuffer);
	FCYSAFEKILL(m_pBackBuffer);
	FCYSAFEKILL(m_pBackDepthBuffer);

	// ɾ����Ⱦ��������
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		ListenerNode* p = pListener;

		pListener = pListener->pNext;

		{
			// ������ܵĶ���й©
			char tTextBuffer[256];
			sprintf_s(tTextBuffer, "Unrelease listener object at %x", p);
#ifdef _DEBUG
			fcyDebug::Trace("[ @ f2dRenderDeviceImpl::~f2dRenderDeviceImpl ] %s\n", tTextBuffer);
#endif
			m_pEngine->ThrowException(fcyException("f2dRenderDeviceImpl::~f2dRenderDeviceImpl", tTextBuffer));
		}

		delete p;
	}
	
	// �ͷŶ�������
	m_VDCache.clear();

	FCYSAFEDEL(m_pSyncTestObj);

	// �ͷ�DX���
	FCYSAFEKILL(m_pWinSurface);
	FCYSAFEKILL(m_pDev);
	FCYSAFEKILL(m_pD3D9);
}

HRESULT f2dRenderDeviceImpl::doReset(D3DPRESENT_PARAMETERS* pD3DPP)
{
	struct _Work : public fcyRefObjImpl<f2dMainThreadDelegate>
	{
		IDirect3DDevice9* pDev;
		D3DPRESENT_PARAMETERS* D3DPP;
		HRESULT HR;
		
		void Excute() { HR = pDev->Reset(D3DPP); }

		_Work(IDirect3DDevice9* p, D3DPRESENT_PARAMETERS* pp)
			: pDev(p), D3DPP(pp), HR(D3DERR_DEVICELOST) {}
	};

	if(GetCurrentThreadId() != m_CreateThreadID)
	{
		HRESULT tHR;
		_Work* tWork = new _Work(m_pDev, pD3DPP);

		m_pEngine->InvokeDelegateAndWait(tWork);

		tHR = tWork->HR;
		FCYSAFEKILL(tWork);

		return tHR;
	}
	else
	{
		return m_pDev->Reset(pD3DPP);
	}
}

HRESULT f2dRenderDeviceImpl::doTestCooperativeLevel()
{
	if(GetCurrentThreadId() != m_CreateThreadID)
	{
		m_pSyncTestObj->Reset();
		m_pEngine->InvokeDelegateAndWait(m_pSyncTestObj);
		return m_pSyncTestObj->GetResult();
	}
	else
	{
		return m_pDev->TestCooperativeLevel();
	}
}

void f2dRenderDeviceImpl::initState()
{
	// --- ��ʼ���ӿ� ---
	m_pDev->SetViewport(&m_ViewPort);

	// --- ����Ĭ����Ⱦ״̬ ---
	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);   // ���÷����޳�
	m_pDev->SetRenderState(D3DRS_LIGHTING, FALSE);         // �رչ���
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);           // ����Z����
	m_pDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE); // �򿪾��βü�����

	// --- ����ZBUFFER ---
	m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE); 
	m_pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	// --- ����Ĭ�ϻ��״̬ ---
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  // ����Alpha���
	m_pDev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);

	m_pDev->SetRenderState(D3DRS_BLENDOP, m_CurBlendState.BlendOp);
	m_pDev->SetRenderState(D3DRS_SRCBLEND, m_CurBlendState.SrcBlend);
	m_pDev->SetRenderState(D3DRS_DESTBLEND, m_CurBlendState.DestBlend);
	m_pDev->SetRenderState(D3DRS_BLENDOPALPHA, m_CurBlendState.AlphaBlendOp);
	m_pDev->SetRenderState(D3DRS_SRCBLENDALPHA, m_CurBlendState.AlphaSrcBlend);
	m_pDev->SetRenderState(D3DRS_DESTBLENDALPHA, m_CurBlendState.AlphaDestBlend);

	// --- �����ϲ���Ĭ��ֵ ---
	m_pDev->SetTextureStageState(0, D3DTSS_COLOROP,  m_CurTexBlendOP_Color);    
	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	// --- ����Ѱַ ---
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// --- ���ò����� ---
	// ������
	m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// --- �������� ---
	// ���þ��βü�
	m_pDev->SetScissorRect(&m_ScissorRect);

	// ����Ĭ�Ͼ���
	m_pDev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_CurWorldMat);
	m_pDev->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_CurLookatMat);
	m_pDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_CurProjMat);
}

int f2dRenderDeviceImpl::sendDevLostMsg()
{
	int tRet = 0;

	m_pCurVertDecl = NULL;

	// �ͷſ��ܵĶ���
	FCYSAFEKILL(m_pCurBackBuffer);
	FCYSAFEKILL(m_pCurBackDepthBuffer);
	FCYSAFEKILL(m_pBackBuffer);
	FCYSAFEKILL(m_pBackDepthBuffer);
	
	// ���Ͷ�ʧ��Ϣ
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		pListener->pListener->OnRenderDeviceLost();
		pListener = pListener->pNext;
		
		tRet++;
	}

	return tRet;
}

int f2dRenderDeviceImpl::sendDevResetMsg()
{
	int tRet = 0;

	// ����������Ϣ
	ListenerNode* pListener = m_ListenerList;
	while(pListener)
	{
		pListener->pListener->OnRenderDeviceReset();
		pListener = pListener->pNext;

		tRet++;
	}

	return tRet;
}

fResult f2dRenderDeviceImpl::SyncDevice()
{
	HRESULT tHR = doTestCooperativeLevel();	// �豸Э������
	
	if(m_bDevLost) // �豸�Ѿ���ʧ
	{	
		if(tHR == D3DERR_DEVICENOTRESET) // ��δ����
		{
			tHR = doReset(&m_D3Dpp);

			if(SUCCEEDED(tHR)) // ���óɹ�
			{
				m_bDevLost = false;

				initState(); // ���³�ʼ��״̬

				int tObjCount = sendDevResetMsg(); // ֪ͨ���й�����ָ�����

				fChar tBuffer[256];
				sprintf_s(tBuffer, "Device reseted. ( %d Object(s) rested. )", tObjCount);
				m_pEngine->ThrowException(fcyException("f2dRenderDeviceImpl::SyncDevice", tBuffer));
			}
		}
	}
	else if(tHR == D3DERR_DEVICELOST || tHR == D3DERR_DEVICENOTRESET) 	// �豸��δ��ʧ������Ƿ�ʧ
	{
		m_bDevLost = true; // ����豸��ʧ
		
		int tObjCount = sendDevLostMsg();

		fChar tBuffer[256];
		sprintf_s(tBuffer, "Detected device lost. ( %d Object(s) losted. )", tObjCount);
		m_pEngine->ThrowException(fcyException("f2dRenderDeviceImpl::SyncDevice", tBuffer));
	}

	return (m_bDevLost ? FCYERR_INTERNALERR : FCYERR_OK);
}

fResult f2dRenderDeviceImpl::Present()
{
	// ��ԭ��ǰ�ĺ�̨������
	if(m_pBackBuffer)
	{
		m_pDev->SetRenderTarget(0, m_pBackBuffer);
		FCYSAFEKILL(m_pBackBuffer);
		FCYSAFEKILL(m_pCurBackBuffer);
	}
	if(m_pBackDepthBuffer)
	{
		m_pDev->SetDepthStencilSurface(m_pBackDepthBuffer);
		FCYSAFEKILL(m_pBackDepthBuffer);
		FCYSAFEKILL(m_pCurBackDepthBuffer);
	}

	if(FAILED(m_pDev->Present(0,0,0,0)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitCurGraphics(f2dGraphics* pGraph, bool bDirty)
{
	if(pGraph == NULL)
	{
		m_pCurGraphics = NULL;
		return FCYERR_OK;
	}
	else if(m_pCurGraphics)
	{
		if(m_pCurGraphics->IsInRender())
			return FCYERR_ILLEGAL;
	}

	if(!bDirty && pGraph == m_pCurGraphics)
		return FCYERR_OK;

	// ����״̬
	if(!pGraph->IsGraphics3D())
	{
		SubmitWorldMat(pGraph->GetWorldTransform());
		SubmitLookatMat(pGraph->GetViewTransform());
		SubmitProjMat(pGraph->GetProjTransform());
		m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);   // ����Ϊ����Ϊ2D��Ⱦ������ر��޳�
	}
	else
		m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   // ����Ϊ����Ϊ3D��Ⱦ������������ʱ���޳�

	SubmitBlendState(pGraph->GetBlendState());

	m_pCurGraphics = pGraph;

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitWorldMat(const fcyMatrix4& Mat)
{
	m_CurWorldMat = Mat;

	if(FCYFAILED(m_pDev->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Mat)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitLookatMat(const fcyMatrix4& Mat)
{
	m_CurLookatMat = Mat;

	if(FCYFAILED(m_pDev->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Mat)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitProjMat(const fcyMatrix4& Mat)
{
	m_CurProjMat = Mat;

	if(FCYFAILED(m_pDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&Mat)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitBlendState(const f2dBlendState& State)
{
	// �Ա�״̬������
	if(m_CurBlendState.AlphaBlendOp != State.AlphaBlendOp)
		m_pDev->SetRenderState(D3DRS_BLENDOPALPHA, State.AlphaBlendOp);
	if(m_CurBlendState.AlphaDestBlend != State.AlphaDestBlend)
		m_pDev->SetRenderState(D3DRS_DESTBLENDALPHA, State.AlphaDestBlend);
	if(m_CurBlendState.AlphaSrcBlend != State.AlphaSrcBlend)
		m_pDev->SetRenderState(D3DRS_SRCBLENDALPHA, State.AlphaSrcBlend);
	if(m_CurBlendState.BlendOp != State.BlendOp)
		m_pDev->SetRenderState(D3DRS_BLENDOP, State.BlendOp);
	if(m_CurBlendState.DestBlend != State.DestBlend)
		m_pDev->SetRenderState(D3DRS_DESTBLEND, State.DestBlend);
	if(m_CurBlendState.SrcBlend != State.SrcBlend)
		m_pDev->SetRenderState(D3DRS_SRCBLEND, State.SrcBlend);

	m_CurBlendState = State;

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SubmitVD(IDirect3DVertexDeclaration9* pVD)
{
	if(m_pCurVertDecl == pVD)
		return FCYERR_OK;
	else
	{
		m_pCurVertDecl = pVD;
		if(FAILED(m_pDev->SetVertexDeclaration(pVD)))
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
	}
}

fResult f2dRenderDeviceImpl::SubmitTextureBlendOP_Color(D3DTEXTUREOP ColorOP)
{
	if(m_CurTexBlendOP_Color == ColorOP)
		return FCYERR_OK;
	else
	{
		if(FAILED(m_pDev->SetTextureStageState(0, D3DTSS_COLOROP, ColorOP)))
			return FCYERR_INTERNALERR;
		else
		{
			m_CurTexBlendOP_Color = ColorOP;
			return FCYERR_OK;
		}
	}
}

IDirect3DVertexDeclaration9* f2dRenderDeviceImpl::RegisterVertexDeclare(f2dVertexElement* pElement, fuInt ElementCount, fuInt& ElementSize)
{
	if(ElementCount == 0)
		return NULL;

	// === Hash��� ===
	fuInt HashCode = fcyHash::SuperFastHash((fcData)pElement, sizeof(f2dVertexElement) * ElementCount);
	
	for(fuInt i = 0; i<m_VDCache.size(); ++i)
	{
		if(m_VDCache[i].ElementData.size() == ElementCount && m_VDCache[i].Hash == HashCode)
		{
			fBool tHas = true;

			vector<f2dVertexElement>& tVec = m_VDCache[i].ElementData;
			for(fuInt j = 0; j<tVec.size(); j++)
			{
				if(memcmp(&tVec[j], &pElement[j], sizeof(f2dVertexElement))!=0)
				{
					tHas = false;
					break;
				}
			}

			/*
				��л��ڤ������issue
			*/
			if (tHas)
			{
				ElementSize = m_VDCache[i].VertexSize;
				return m_VDCache[i].pVertexDeclare;
			}
		}
	}

	// === ������Ŀ ===
	D3DVERTEXELEMENT9* pElementArr = new D3DVERTEXELEMENT9[ ElementCount + 1 ];
	ZeroMemory(pElementArr, sizeof(D3DVERTEXELEMENT9) * (ElementCount + 1));

	// ��β
	D3DVERTEXELEMENT9 tEnd = D3DDECL_END();
	pElementArr[ ElementCount ] = tEnd;

	fuInt tOffset = 0;

	for(fuInt i = 0; i<ElementCount; ++i)
	{
		pElementArr[i].Stream = 0;
		pElementArr[i].Offset = tOffset;
		pElementArr[i].Type = pElement[i].Type;
		pElementArr[i].Method = D3DDECLMETHOD_DEFAULT;
		pElementArr[i].Usage = pElement[i].Usage;
		pElementArr[i].UsageIndex = pElement[i].UsageIndex;

		switch(pElementArr[i].Type)
		{
		case D3DDECLTYPE_FLOAT1:
			tOffset += sizeof(float);
			break;
		case D3DDECLTYPE_FLOAT2:
			tOffset += sizeof(float) * 2;
			break;
		case D3DDECLTYPE_FLOAT3:
			tOffset += sizeof(float) * 3;
			break;
		case D3DDECLTYPE_FLOAT4:
			tOffset += sizeof(float) * 4;
			break;
		case D3DDECLTYPE_D3DCOLOR:
			tOffset += sizeof(char) * 4;
			break;
		case D3DDECLTYPE_UBYTE4:
			tOffset += sizeof(char) * 4;
			break;
		case D3DDECLTYPE_SHORT2:
			tOffset += sizeof(short) * 2;
			break;
		case D3DDECLTYPE_SHORT4:
			tOffset += sizeof(short) * 4;
			break;
		}
	}

	IDirect3DVertexDeclaration9* pVD = NULL;
	HRESULT tHR = m_pDev->CreateVertexDeclaration(pElementArr, &pVD);
	FCYSAFEDELARR(pElementArr);

	if(FAILED(tHR))
		return NULL;

	VertexDeclareInfo tInfo;
	tInfo.Hash = HashCode;
	tInfo.pVertexDeclare = pVD;
	tInfo.ElementData.resize(ElementCount);
	tInfo.VertexSize = tOffset;
	for(fuInt i = 0; i<ElementCount; ++i)
	{
		tInfo.ElementData[i] = pElement[i];
	}
	m_VDCache.push_back(tInfo);

	ElementSize = tOffset;

	return pVD;
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

fuInt f2dRenderDeviceImpl::GetSupportResolutionCount()
{
	return m_pD3D9->GetAdapterModeCount(0, D3DFMT_X8R8G8B8);
}

fcyVec2 f2dRenderDeviceImpl::EnumSupportResolution(fuInt Index)
{
	fcyVec2 tRet;
	D3DDISPLAYMODE tMode;

	if(SUCCEEDED(m_pD3D9->EnumAdapterModes(0, D3DFMT_X8R8G8B8, Index, &tMode)))
	{
		tRet.x = (float)tMode.Width;
		tRet.y = (float)tMode.Height;
	}
	
	return tRet;
}

fResult f2dRenderDeviceImpl::SetBufferSize(fuInt Width, fuInt Height, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
{
	// ׼������
	D3DPRESENT_PARAMETERS tD3Dpp;
	ZeroMemory(&tD3Dpp, sizeof(D3DPRESENT_PARAMETERS));

	tD3Dpp.BackBufferCount = 1;                      // ��̨����ҳ�����
	tD3Dpp.BackBufferWidth = Width;	                 // ����ҳ����
	tD3Dpp.BackBufferHeight = Height;                // ����ҳ��߶�
	tD3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;       // ҳ���ʽ
	tD3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;    // ģ���Z�����ʽ
	tD3Dpp.EnableAutoDepthStencil = true;            // �����Զ�����ģ���Z����
	tD3Dpp.Windowed = Windowed;                      // �Ƿ񴰿�
	tD3Dpp.PresentationInterval = VSync? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;	// ��������
	tD3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	tD3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if(AALevel != F2DAALEVEL_NONE)
	{
		tD3Dpp.MultiSampleQuality = 0;

		// ���������
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

	// ��������δ�йܶ���
	m_bDevLost = true;
	sendDevLostMsg();

	// ������Ⱦϵͳ
	HRESULT tHR = doReset(&tD3Dpp);
	if(FAILED(tHR))
	{
		// ����ʧ�ܣ�����Ϊ��ʧ״̬���ȴ�ѭ��ʱ�ָ��ϴ�״̬��
		return FCYERR_INVAILDPARAM;
	}
	else
	{
		// �ɹ����ָ�����״̬
		m_bDevLost = false;
		m_D3Dpp = tD3Dpp;

		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = GetBufferWidth();
		m_ScissorRect.bottom = GetBufferHeight();

		initState();

		sendDevResetMsg();

		return FCYERR_OK;
	}
}

fResult f2dRenderDeviceImpl::AttachListener(f2dRenderDeviceEventListener* Listener, fInt Priority)
{
	if(Listener == NULL)
		return FCYERR_INVAILDPARAM;

	ListenerNode* pNew = new ListenerNode();
	pNew->pListener = Listener;
	pNew->Priority = Priority;
	pNew->pNext = m_ListenerList;

	m_ListenerList = pNew;

	// �����ȼ����в�������
	ListenerNode* p = m_ListenerList;
	while(p->pNext && p->Priority > p->pNext->Priority)
	{
		ListenerNode* t = p->pNext;

		std::swap(p->pListener, t->pListener);
		std::swap(p->Priority, t->Priority);

		p = t;
	}

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

fResult f2dRenderDeviceImpl::CreateTextureFromStream(f2dStream* pStream, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		if(IsDynamic)
			*pOut = new f2dTexture2DDynamic(this, pStream, Width, Height);
		else
			*pOut = new f2dTexture2DStatic(this, pStream, Width, Height, HasMipmap);
	}
	catch (const std::bad_alloc&)
	{
		return FCYERR_OUTOFMEM;
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateTextureFromMemory(fcData pMemory, fLen Size, fuInt Width, fuInt Height, fBool IsDynamic, fBool HasMipmap, f2dTexture2D** pOut)
{
	if (!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		if (IsDynamic)
			*pOut = new f2dTexture2DDynamic(this, pMemory, Size, Width, Height);
		else
			*pOut = new f2dTexture2DStatic(this, pMemory, Size, Width, Height, HasMipmap);
	}
	catch (const std::bad_alloc&)
	{
		return FCYERR_OUTOFMEM;
	}
	catch (const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateDynamicTexture(fuInt Width, fuInt Height, f2dTexture2D** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dTexture2DDynamic(this, Width, Height);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateRenderTarget(fuInt Width, fuInt Height, fBool AutoResize, f2dTexture2D** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dTexture2DRenderTarget(this, Width, Height, AutoResize);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateDepthStencilSurface(fuInt Width, fuInt Height, fBool Discard, fBool AutoResize, f2dDepthStencilSurface** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dDepthStencilSurfaceImpl(this, Width, Height, AutoResize, Discard);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateGraphics2D(fuInt VertexBufferSize, fuInt IndexBufferSize, f2dGraphics2D** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

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

fResult f2dRenderDeviceImpl::CreateGraphics3D(f2dEffect* pDefaultEffect, f2dGraphics3D** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dGraphics3DImpl(this, (f2dEffectImpl*)pDefaultEffect);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateEffect(f2dStream* pStream, fBool bAutoState, f2dEffect** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dEffectImpl(this, pStream, bAutoState);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::CreateMeshData(f2dVertexElement* pVertElement, fuInt ElementCount, fuInt VertCount, fuInt IndexCount, fBool Int32Index, f2dMeshData** pOut)
{
	if(!pOut)
		return FCYERR_INVAILDPARAM;
	*pOut = NULL;

	try
	{
		*pOut = new f2dMeshDataImpl(this, pVertElement, ElementCount, VertCount, IndexCount, Int32Index);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::Clear(const fcyColor& BackBufferColor, fFloat ZValue)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, BackBufferColor.argb, ZValue, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::Clear(const fcyColor& BackBufferColor, fFloat ZValue, fuInt StencilValue)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET | D3DCLEAR_STENCIL, BackBufferColor.argb, ZValue, StencilValue))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::ClearColor(const fcyColor& BackBufferColor)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, BackBufferColor.argb, 1.f, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::ClearZBuffer(fFloat Value)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, Value, 0))?FCYERR_INTERNALERR:FCYERR_OK;
}

fResult f2dRenderDeviceImpl::ClearStencilBuffer(fuInt StencilValue)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	return FAILED(m_pDev->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, StencilValue))?FCYERR_INTERNALERR:FCYERR_OK;
}

f2dTexture2D* f2dRenderDeviceImpl::GetRenderTarget()
{
	return m_pCurBackBuffer;
}

fResult f2dRenderDeviceImpl::SetRenderTarget(f2dTexture2D* pTex)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	if(m_pCurBackBuffer == pTex)
		return FCYERR_OK;

	if(!pTex)
	{
		FCYSAFEKILL(m_pCurBackBuffer);
		m_pDev->SetRenderTarget(0, m_pBackBuffer);
		FCYSAFEKILL(m_pBackBuffer);
		return FCYERR_OK;
	}
	else if(!pTex->IsRenderTarget())
		return FCYERR_INVAILDPARAM;

	FCYSAFEKILL(m_pCurBackBuffer);
	m_pCurBackBuffer = pTex;
	m_pCurBackBuffer->AddRef();
	if(m_pBackBuffer == NULL)
		m_pDev->GetRenderTarget(0, &m_pBackBuffer);

	m_pDev->SetRenderTarget(0, ((f2dTexture2DRenderTarget*)m_pCurBackBuffer)->GetSurface());

	return FCYERR_OK;
}

f2dDepthStencilSurface* f2dRenderDeviceImpl::GetDepthStencilSurface()
{
	return m_pCurBackDepthBuffer;
}

fResult f2dRenderDeviceImpl::SetDepthStencilSurface(f2dDepthStencilSurface* pSurface)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	if(m_pCurBackDepthBuffer == pSurface)
		return FCYERR_OK;

	if(!pSurface)
	{
		FCYSAFEKILL(m_pCurBackDepthBuffer);
		m_pDev->SetDepthStencilSurface(m_pBackDepthBuffer);
		FCYSAFEKILL(m_pBackDepthBuffer);
		return FCYERR_OK;
	}

	FCYSAFEKILL(m_pCurBackDepthBuffer);
	m_pCurBackDepthBuffer = pSurface;
	m_pCurBackDepthBuffer->AddRef();
	if(m_pBackDepthBuffer == NULL)
		m_pDev->GetDepthStencilSurface(&m_pBackDepthBuffer);

	m_pDev->SetDepthStencilSurface((IDirect3DSurface9*)pSurface->GetHandle());

	return FCYERR_OK;
}

fcyRect f2dRenderDeviceImpl::GetScissorRect()
{
	fcyRect tRet((float)m_ScissorRect.left, (float)m_ScissorRect.top, (float)m_ScissorRect.right, (float)m_ScissorRect.bottom);
	return tRet;
}

fResult f2dRenderDeviceImpl::SetScissorRect(const fcyRect& pRect)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	m_ScissorRect.left = (int)pRect.a.x;
	m_ScissorRect.top = (int)pRect.a.y;
	m_ScissorRect.right = (int)pRect.b.x;
	m_ScissorRect.bottom = (int)pRect.b.y;

	if(FAILED(m_pDev->SetScissorRect(&m_ScissorRect)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fcyRect f2dRenderDeviceImpl::GetViewport()
{
	return fcyRect(
		(float)m_ViewPort.X,
		(float)m_ViewPort.Y,
		(float)(m_ViewPort.X + m_ViewPort.Width),
		(float)(m_ViewPort.Y + m_ViewPort.Height)
	);
}

fResult f2dRenderDeviceImpl::SetViewport(fcyRect vp)
{
	if (m_pCurGraphics && m_pCurGraphics->IsInRender())
		m_pCurGraphics->Flush();

	if (!vp.Intersect(fcyRect(0, 0, (float)GetBufferWidth(), (float)GetBufferHeight()), &vp))
		return FCYERR_ILLEGAL;

	DWORD tNewX = (DWORD)vp.a.x;
	DWORD tNewY = (DWORD)vp.a.y;
	DWORD tNewW = (DWORD)vp.GetWidth();
	DWORD tNewH = (DWORD)vp.GetHeight();

	if (tNewX != m_ViewPort.X || tNewY != m_ViewPort.Y || tNewW != m_ViewPort.Width || tNewH != m_ViewPort.Height)
	{
		m_ViewPort.X = tNewX;
		m_ViewPort.Y = tNewY;
		m_ViewPort.Width = tNewW;
		m_ViewPort.Height = tNewH;

		if (FAILED(m_pDev->SetViewport(&m_ViewPort)))
			return FCYERR_INTERNALERR;
	}
	return FCYERR_OK;
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

	// ���浽��
	if(FCYFAILED(pStream->WriteBytes((fcData)pDataBuffer->GetBufferPointer(), pDataBuffer->GetBufferSize(), NULL)))
	{
		FCYSAFEKILL(pDataBuffer);
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pDataBuffer);

	return FCYERR_OK;
}

fResult f2dRenderDeviceImpl::SaveTexture(f2dStream* pStream, f2dTexture2D* pTex)
{
	if(m_bDevLost)
		return FCYERR_ILLEGAL;

	if(!pStream || !pTex || !pStream->CanWrite() || !pStream->CanResize())
		return FCYERR_INVAILDPARAM;

	IDirect3DSurface9* pSurface = NULL;
	if(pTex->IsDynamic())
		((IDirect3DTexture9*)((f2dTexture2DDynamic*)pTex)->GetHandle())->GetSurfaceLevel(0, &pSurface);
	else if(pTex->IsRenderTarget())
	{
		pSurface = ((f2dTexture2DRenderTarget*)pTex)->GetSurface();
		pSurface->AddRef();
	}
	else
		((IDirect3DTexture9*)((f2dTexture2DStatic*)pTex)->GetHandle())->GetSurfaceLevel(0, &pSurface);

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

	// ���浽��
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

	// === ��鴰�ڴ�С�Ƿ�ƥ�� ===
	if(m_DC.GetSafeHdc() == NULL || m_pWinSurface == NULL || (fuInt)m_DC.GetWidth() != tBackWidth || (fuInt)m_DC.GetHeight() != tBackHeight)
	{
		m_DC.Delete();
		m_DC.Create(tBackWidth, tBackHeight);

		// ��������ҳ��
		FCYSAFEKILL(m_pWinSurface);
		if(FAILED(m_pDev->CreateOffscreenPlainSurface(tBackWidth, tBackHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pWinSurface, NULL)))
			return FCYERR_INTERNALERR;
	}

	// === ׼������ ===
	RECT tWinRect;
	::GetWindowRect(m_hWnd, &tWinRect);
	HDC tWinDC = GetWindowDC(m_hWnd);
	// ����λ��
	POINT ptWinPos = { tWinRect.left, tWinRect.top };
	// DC������ʼλ��
	POINT ptSrc = { 0, 0 };
	// ���ڴ�С
	SIZE szWin = { tWinRect.right - tWinRect.left, tWinRect.bottom - tWinRect.top };
	// DC�����С
	RECT tDCSurface = { 0, 0, tBackWidth, tBackHeight };
	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, Alpha, AC_SRC_ALPHA };
	
	// === ����BackSurface ===
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

	// === ������GDI ===
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
