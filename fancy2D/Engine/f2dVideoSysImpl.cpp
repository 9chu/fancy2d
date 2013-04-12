#include "f2dVideoSysImpl.h"

#include "f2dEngineImpl.h"

#include <stack>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
const GUID f2dVideoRenderer::GUID_f2dVideoRenderer = 
{ 0x598256bc, 0x185b, 0x4f8c, { 0x98, 0x81, 0xda, 0x12, 0xab, 0x50, 0xcc, 0x74 } };

f2dVideoRenderer::f2dVideoRenderer(HRESULT *phr)
	: m_TexWidth(0), m_TexHeight(0), m_RawDataType(SUPPORTMEDIATYPE_RGB24), m_lVideoWidth(0), m_lVideoHeight(0), m_lVideoPitch(0),
	m_BufferFlag(0),
	m_pListener(NULL), m_TimeTotal(0), m_DataTotal(0), m_FPSTotal(0), m_DataPerSecond(0), m_FPS(0),
	CBaseVideoRenderer(GUID_f2dVideoRenderer, L"f2dVideoRenderer", NULL, phr)
{
	m_TexBuffer[0] = m_TexBuffer[1] = NULL;
}

f2dVideoRenderer::~f2dVideoRenderer()
{
	FCYSAFEDELARR(m_TexBuffer[1]);
	FCYSAFEDELARR(m_TexBuffer[0]);
}

void f2dVideoRenderer::renderRGBSample(fcData pVideo, fcyColor* pTex)
{
	// 由于视频数据从底部向上保存，先偏移到最后一行
	pTex += (m_lVideoWidth * (m_lVideoHeight-1));

	// 拷贝视频数据
	for(fuInt y=0; y<m_lVideoHeight; y++)
	{
		fuInt SrcPos = 0;
		
		// 复制行
		for(fuInt x=0; x<m_lVideoWidth; x++) 
		{
			switch(m_RawDataType)
			{
			case SUPPORTMEDIATYPE_RGB24:
				pTex->b = pVideo[SrcPos++];  // B
				pTex->g = pVideo[SrcPos++];  // G
				pTex->r = pVideo[SrcPos++];  // R
				pTex->a = 0xFF;              // A
				break;
			case SUPPORTMEDIATYPE_RGB32:
				pTex->b = pVideo[SrcPos++];  // B
				pTex->g = pVideo[SrcPos++];  // G
				pTex->r = pVideo[SrcPos++];  // R
				pTex->a = 0xFF;              // A
				SrcPos++;
				break;
			case SUPPORTMEDIATYPE_ARGB32:
				pTex->b = pVideo[SrcPos++];  // B
				pTex->g = pVideo[SrcPos++];  // G
				pTex->r = pVideo[SrcPos++];  // R
				pTex->a = pVideo[SrcPos++];  // A
				break;
			}

			if(m_pListener)
				m_pListener->OnVideoDrawPixel(x, y, pTex);

			pTex++;
		}

		// 移动到下一行
		pVideo += m_lVideoPitch;
		pTex -= m_lVideoWidth << 1;
	}
}

void f2dVideoRenderer::renderYUY2Sample(fcData pVideo, fcyColor* pTex)
{
	fLen tDataCount = m_lVideoWidth * m_lVideoHeight / 2;

	// 拷贝并转换YUY2数据
	// B = 1.164(Y - 16) + 2.018(U - 128)
	// G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
	// R = 1.164(Y - 16) + 1.596(V - 128)
	if(m_pListener)
	{
		fuInt tX = 0, tY = 0;

		for(fLen i = 0; i<tDataCount; i++)
		{
			// Y0 U0 Y1 V0 数据
			fByte Y0 = *(pVideo++);
			fByte U = *(pVideo++);
			fByte Y1 = *(pVideo++);
			fByte V = *(pVideo++);

			fInt tY;
			fInt tU = U - 128;
			fInt tV = V - 128;

			tY = Y0 - 16;
			pTex->b = clip255(( 298 * tY + 516 * tU            + 128) >> 8);
			pTex->g = clip255(( 298 * tY - 100 * tU - 208 * tV + 128) >> 8);
			pTex->r = clip255(( 298 * tY            + 409 * tV + 128) >> 8);
			pTex->a = 0xFF;
			
			m_pListener->OnVideoDrawPixel(tX, tY, pTex);
			tX++;
			if(tX >= m_lVideoWidth)
			{
				tX = 0;
				tY++;
			}
			pTex++;

			tY = Y1 - 16;
			pTex->b = clip255(( 298 * tY + 516 * tU            + 128) >> 8);
			pTex->g = clip255(( 298 * tY - 100 * tU - 208 * tV + 128) >> 8);
			pTex->r = clip255(( 298 * tY            + 409 * tV + 128) >> 8);
			pTex->a = 0xFF;

			m_pListener->OnVideoDrawPixel(tX, tY, pTex);
			tX++;
			if(tX >= m_lVideoWidth)
			{
				tX = 0;
				tY++;
			}
			pTex++;
		}
	}
	else
	{	
		for(fLen i = 0; i<tDataCount; i++)
		{
			// Y0 U0 Y1 V0 数据
			fByte Y0 = *(pVideo++);
			fByte U = *(pVideo++);
			fByte Y1 = *(pVideo++);
			fByte V = *(pVideo++);

			fInt tY;
			fInt tU = U - 128;
			fInt tV = V - 128;

			tY = Y0 - 16;
			pTex->b = clip255(( 298 * tY + 516 * tU            + 128) >> 8);
			pTex->g = clip255(( 298 * tY - 100 * tU - 208 * tV + 128) >> 8);
			pTex->r = clip255(( 298 * tY            + 409 * tV + 128) >> 8);
			pTex->a = 0xFF;
			pTex++;

			tY = Y1 - 16;
			pTex->b = clip255(( 298 * tY + 516 * tU            + 128) >> 8);
			pTex->g = clip255(( 298 * tY - 100 * tU - 208 * tV + 128) >> 8);
			pTex->r = clip255(( 298 * tY            + 409 * tV + 128) >> 8);
			pTex->a = 0xFF;
			pTex++;
		}
	}
}

f2dVideoRendererListener* f2dVideoRenderer::GetRendererListener()
{
	CAutoLock tLock(&m_Lock);

	f2dVideoRendererListener* pRet = m_pListener;

	return pRet;
}

void f2dVideoRenderer::SetRendererListener(f2dVideoRendererListener* pListener)
{
	CAutoLock tLock(&m_Lock);

	m_pListener = pListener;
}

fcyColor* f2dVideoRenderer::GetInternalPtr(fuInt Index)
{
	CAutoLock tLock(&m_Lock);

	if(Index > 1)
		return NULL;
	
	fcyColor* pColor = m_TexBuffer[Index];

	return pColor;
}

fuInt f2dVideoRenderer::GetVideoWidth()
{
	CAutoLock tLock(&m_Lock);

	fuInt tRet = (fuInt)m_lVideoWidth;

	return tRet;
}

fuInt f2dVideoRenderer::GetVideoHeight()
{
	CAutoLock tLock(&m_Lock);

	fuInt tRet = (fuInt)m_lVideoHeight;

	return tRet;
}

fResult f2dVideoRenderer::CopyDataToTexture(f2dTexture* pTex)
{
	if(!pTex)
		return FCYERR_INVAILDPARAM;
	if(!pTex->IsDynamic() || !m_TexBuffer[0])
		return FCYERR_ILLEGAL;
	if(pTex->GetWidth() < m_lVideoWidth || pTex->GetHeight() < m_lVideoHeight)
		return FCYERR_INVAILDPARAM;

	fInt tIndex = 0;

	{
		// 锁定以获得双缓冲区属性
		m_Lock.Lock();

		tIndex = !m_BufferFlag;

		// 锁定缓冲区
		m_BufferLock[tIndex].Lock();

		m_Lock.Unlock();
	}

	fuInt tPitch = 0;
	fByte* pData = NULL;
	if(FCYFAILED(pTex->Lock(NULL, true, &tPitch, &pData)))
	{
		m_BufferLock[tIndex].Unlock();
		return FCYERR_INTERNALERR;
	}

	fcyColor* pTexData = m_TexBuffer[tIndex];
	for(fuInt Y = 0; Y<m_lVideoHeight; Y++)
	{
		for(fuInt X = 0; X<m_lVideoWidth; X++)
		{
			((fcyColor*)pData)[X] = *pTexData;

			pTexData++;
		}

		pData += tPitch;
	}

	pTex->Unlock();

	// 解锁缓冲区
	m_BufferLock[tIndex].Unlock();

	return FCYERR_OK;
}

fFloat f2dVideoRenderer::GetDataPerSecond()
{
	CAutoLock tLock(&m_Lock);

	fFloat tRet = m_DataPerSecond;

	return tRet;
}

fFloat f2dVideoRenderer::GetVideoRenderFPS()
{
	CAutoLock tLock(&m_Lock);

	fFloat tRet = m_FPS;

	return tRet;
}

HRESULT f2dVideoRenderer::CheckMediaType(const CMediaType *pMediaType)
{
	CAutoLock tLock(&m_Lock);

	if(IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video))
		if(IsEqualGUID(*pMediaType->FormatType(), FORMAT_VideoInfo))
		{
			// RGB颜色
			if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24))
				return S_OK;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32))
				return S_OK;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_ARGB32))
				return S_OK;
			// YUV颜色
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2))
				return S_OK;
		}
		else if(IsEqualGUID(*pMediaType->FormatType(), FORMAT_VideoInfo2))
		{
			// RGB颜色
			if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24))
				return S_OK;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32))
				return S_OK;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_ARGB32))
				return S_OK;
			// YUV颜色
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2))
				return S_OK;
		}

	return E_FAIL;
}

HRESULT f2dVideoRenderer::SetMediaType(const CMediaType *pMediaType)
{
	CAutoLock tLock(&m_Lock);

	HRESULT tHR = S_OK;
	VIDEOINFOHEADER *pVideoInfo = NULL;
	VIDEOINFOHEADER2 *pVideoInfo2 = NULL;

	if(IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video))
	{
		if(IsEqualGUID(*pMediaType->FormatType(), FORMAT_VideoInfo))
		{
			pVideoInfo = (VIDEOINFOHEADER*)pMediaType->Format();

			// RGB颜色
			if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24))
				m_RawDataType = SUPPORTMEDIATYPE_RGB24;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32))
				m_RawDataType = SUPPORTMEDIATYPE_RGB32;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_ARGB32))
				m_RawDataType = SUPPORTMEDIATYPE_ARGB32;
			// YUV颜色
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2))
				m_RawDataType = SUPPORTMEDIATYPE_YUY2;
			else
				return E_FAIL;

			// 计算MediaType数据
			m_lVideoWidth = pVideoInfo->bmiHeader.biWidth;
			m_lVideoHeight = abs(pVideoInfo->bmiHeader.biHeight);
			if(m_RawDataType == SUPPORTMEDIATYPE_RGB24)
				m_lVideoPitch = (m_lVideoWidth * 3 + 3) & ~(3);
			else if(m_RawDataType == SUPPORTMEDIATYPE_RGB32 || m_RawDataType == SUPPORTMEDIATYPE_ARGB32)
				m_lVideoPitch = m_lVideoWidth * 4;
			else if(m_RawDataType == SUPPORTMEDIATYPE_YUY2)
				m_lVideoPitch = m_lVideoWidth * 4;
		}
		else if(IsEqualGUID(*pMediaType->FormatType(), FORMAT_VideoInfo2))
		{
			pVideoInfo2 = (VIDEOINFOHEADER2*)pMediaType->Format();

			// RGB颜色
			if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24))
				m_RawDataType = SUPPORTMEDIATYPE_RGB24;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32))
				m_RawDataType = SUPPORTMEDIATYPE_RGB24;
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_ARGB32))
				m_RawDataType = SUPPORTMEDIATYPE_RGB24;
			// YUV颜色
			else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2))
				m_RawDataType = SUPPORTMEDIATYPE_YUY2;
			else
				return E_FAIL;

			// 计算MediaType数据
			m_lVideoWidth = pVideoInfo2->bmiHeader.biWidth;
			m_lVideoHeight = abs(pVideoInfo2->bmiHeader.biHeight);
			if(m_RawDataType == SUPPORTMEDIATYPE_RGB24)
				m_lVideoPitch = (m_lVideoWidth * 3 + 3) & ~(3);
			else if(m_RawDataType == SUPPORTMEDIATYPE_RGB32 || m_RawDataType == SUPPORTMEDIATYPE_ARGB32)
				m_lVideoPitch = m_lVideoWidth * 4;
			else if(m_RawDataType == SUPPORTMEDIATYPE_YUY2)
				m_lVideoPitch = m_lVideoWidth * 4;
		}
		else
			return E_FAIL;
	}
	else
		return E_FAIL;

	// 创建纹理
	if(!m_TexBuffer[0] || (m_lVideoWidth != m_TexWidth || m_lVideoHeight != m_TexHeight))
	{
		FCYSAFEDELARR(m_TexBuffer[0]);
		FCYSAFEDELARR(m_TexBuffer[1]);

		m_TexWidth = m_lVideoWidth;
		m_TexHeight = m_lVideoHeight;
		m_TexBuffer[0] = new fcyColor[m_lVideoWidth * m_lVideoHeight];
		m_TexBuffer[1] = new fcyColor[m_lVideoWidth * m_lVideoHeight];
	}

	return S_OK;
}

HRESULT f2dVideoRenderer::DoRenderSample(IMediaSample *pMediaSample)
{
	if(!pMediaSample || !m_TexBuffer[0])
		return E_POINTER;

	// 获得视频采样区指针
	fByte *pSamplePtr;
	pMediaSample->GetPointer(&pSamplePtr);

	if(m_pListener)
		m_pListener->OnVideoStartRender();

	// 拷贝RGB数据
	if(m_RawDataType <= SUPPORTMEDIATYPE_ARGB32)
		renderRGBSample(pSamplePtr, m_TexBuffer[m_BufferFlag]);
	// 拷贝YUY2数据
	else if(m_RawDataType == SUPPORTMEDIATYPE_YUY2)
		renderYUY2Sample(pSamplePtr, m_TexBuffer[m_BufferFlag]);

	if(m_pListener)
		m_pListener->OnVideoEndRender();

	// 更新缓冲区标识
	m_Lock.Lock();

	m_BufferFlag = !m_BufferFlag;

	// 如此时在拷贝数据则等待
	m_BufferLock[m_BufferFlag].Lock();
	m_BufferLock[m_BufferFlag].Unlock();

	m_Lock.Unlock();

	m_TimeTotal += (float)m_Watch.GetElpased();
	m_DataTotal += pMediaSample->GetActualDataLength();
	m_FPSTotal ++;
	m_Watch.Reset();
	if(m_TimeTotal >= 1.f)
	{
		m_DataPerSecond = m_DataTotal / m_TimeTotal;
		m_FPS = m_FPSTotal / m_TimeTotal;
		m_TimeTotal = 0;
		m_DataTotal = 0;
		m_FPSTotal = 0;
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dVideoImpl::f2dVideoImpl(f2dVideoSysImpl* pParent, f2dStream* pStream, fuInt Flag)
	: m_ObjectTableEntry(0),
	m_pStream(NULL), m_pReader(NULL),
	m_pFilterGraph(NULL), m_pMediaControl(NULL), m_pMediaEvent(NULL), m_pMediaSeek(NULL),
	m_pVideoStep(NULL), m_pAudioControl(NULL),
	m_pRenderer(NULL), m_pTex(NULL),
	m_bLoop(false)
{
	HRESULT tHR = S_OK;

	// --- 构建流 ---
	m_pStream = new f2dVideoStream(pStream);
	m_pReader = new f2dVideoStreamReader(m_pStream, &tHR);
	if(FAILED(tHR))
	{
		FCYSAFEDEL(m_pReader);
		FCYSAFEDEL(m_pStream);

		throw fcyWin32COMException("f2dVideoImpl::f2dVideoImpl", "Create Stream Failed.", tHR);
	}

	m_pReader->AddRef();

	// --- 创建过滤器管理器 ---
	if(FAILED(tHR = 
		CoCreateInstance(
			CLSID_FilterGraph, 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			IID_IGraphBuilder, 
			(void **)&m_pFilterGraph
			)))
	{
		clearUp();
		throw fcyWin32COMException("f2dVideoImpl::f2dVideoImpl", "CoCreateInstance(CLSID_FilterGraph) Failed.", tHR);
	}
	
	// 查询接口，获得媒体控制和媒体事件接口
	m_pFilterGraph->QueryInterface(IID_IMediaControl,  (void**)&m_pMediaControl);
	m_pFilterGraph->QueryInterface(IID_IMediaEventEx,  (void**)&m_pMediaEvent);
	m_pFilterGraph->QueryInterface(IID_IMediaSeeking, (void**)&m_pMediaSeek);
	m_pFilterGraph->QueryInterface(IID_IVideoFrameStep,(void**)&m_pVideoStep);
	m_pFilterGraph->QueryInterface(IID_IBasicAudio,    (void**)&m_pAudioControl);

	// --- 设置时间单位 ---
	m_pMediaSeek->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);

	// --- 挂接调试器 ---
	if(Flag & F2DVIDEOFLAG_DEBUG)
		registerDebug();

	// --- 加载Filter ---
	for(fuInt i = 0; i<pParent->m_FliterGUIDList.size(); i++)
		addFilter(pParent->m_FliterGUIDList[i]);

	// --- 挂接视频渲染器 ---
	tHR = S_OK;
	m_pRenderer = new f2dVideoRenderer(&tHR);
	if(FAILED(tHR))
	{
		FCYSAFEDEL(m_pRenderer);
		clearUp();
		throw fcyWin32COMException("f2dVideoImpl::f2dVideoImpl", "f2dVideoRenderer Init Failed.", tHR);
	}
	m_pRenderer->AddRef();
	
	tHR = m_pFilterGraph->AddFilter(m_pRenderer, L"f2dVideoRenderer");
	if(FAILED(tHR))
	{
		clearUp();
		throw fcyWin32COMException("f2dVideoImpl::f2dVideoImpl", "IGraphBuilder::AddFilter Failed.", tHR);
	}

	// --- 挂接流 ---
	m_pFilterGraph->AddFilter(m_pReader, L"f2dVideoStream");

	// --- 渲染流 ---
	if(FAILED(tHR = m_pFilterGraph->Render(m_pReader->GetPin(0))))
	{
		clearUp();
		throw fcyWin32COMException("f2dVideoImpl::f2dVideoImpl", "IGraphBuilder::Render Failed.", tHR);
	}
	
	// --- 检查视频流是否已被选用，若没有，返回失败 ---
	if(m_pRenderer->GetInternalPtr(0) == NULL)
	{
		clearUp();
		throw fcyException("f2dVideoImpl::f2dVideoImpl", "Video Render Connect Failed.");
	}

	// --- 创建内部纹理 ---
	if(! (Flag & F2DVIDEOFLAG_NOMANAGEDTEX))
	{
		if(FCYFAILED(pParent->GetRenderDevice()->CreateDynamicTexture(
			m_pRenderer->GetVideoWidth(),
			m_pRenderer->GetVideoHeight(),
			&m_pTex
			)))
		{
			clearUp();
			throw fcyException("f2dVideoImpl::f2dVideoImpl", "CreateDynamicTexture Failed.");
		}
	}

	// --- 清除没用的Filter ---
	clearUselessFilter();
}

f2dVideoImpl::~f2dVideoImpl()
{
	clearUp();
}

void f2dVideoImpl::clearUp()
{
	// 如果已在播放则先终止
	if(m_pMediaControl)
		m_pMediaControl->Stop();

	// 卸载调试器
	if(m_ObjectTableEntry)
		unregisterDebug();

	// 卸载DShow组件
	FCYSAFEKILL(m_pAudioControl);
	FCYSAFEKILL(m_pVideoStep);
	FCYSAFEKILL(m_pMediaSeek);
	FCYSAFEKILL(m_pMediaEvent);
	FCYSAFEKILL(m_pMediaControl);
	FCYSAFEKILL(m_pFilterGraph);

	// 卸载纹理
	FCYSAFEKILL(m_pTex);

	// 卸载视频对象
	FCYSAFEKILL(m_pRenderer);

	// 卸载流
	FCYSAFEDEL(m_pStream);
	FCYSAFEKILL(m_pReader);
}

void f2dVideoImpl::registerDebug()
{
	IMoniker * pMoniker = 0;
    IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
		WCHAR wsz[256];
		swprintf_s(wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)m_pFilterGraph, GetCurrentProcessId());
		HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
		if (SUCCEEDED(hr)) 
		{
			hr = objectTable->Register(0, m_pFilterGraph, pMoniker, &m_ObjectTableEntry);
			pMoniker->Release();
		}
		objectTable->Release();
	}
}

void f2dVideoImpl::unregisterDebug()
{
	IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
        objectTable->Revoke(m_ObjectTableEntry);
        objectTable->Release();
		m_ObjectTableEntry = 0;
    }
}

fBool f2dVideoImpl::addFilter(const f2dVideoFilterInfo& Filter)
{
	IBaseFilter* pFilter = NULL;
	HRESULT tHR = S_OK;

	if(Filter.hDLLInstance == NULL)
	{
		tHR = CoCreateInstance(Filter.FilterGUID, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFilter);
		if(FAILED(tHR))
			return false;
	}
	else
	{
		BOOL tRet = f2dVideoSysImpl::createInstanceFromDll(Filter.hDLLInstance, Filter.FilterGUID, IID_IBaseFilter, (void**)&pFilter);
		if(tRet == FALSE)
			return false;
	}

	tHR = m_pFilterGraph->AddFilter(pFilter, NULL);

	FCYSAFEKILL(pFilter);

	return SUCCEEDED(tHR);
}

void f2dVideoImpl::clearUselessFilter()
{
	IEnumFilters* pEnum = NULL;
	IBaseFilter* pFilter = NULL;
	ULONG tFetchCount = 0;
	stack<IBaseFilter*> tFilterToRemove;

	if(FAILED(m_pFilterGraph->EnumFilters(&pEnum)))
		return;

	while(SUCCEEDED(pEnum->Next(1, &pFilter, &tFetchCount)))
	{
		if(tFetchCount == 0)
			break;

		IEnumPins* pEnumPins = NULL;
		IPin* pPin = NULL;
		ULONG tFetchPinCount = 0;
		fBool bConnected = false;

		if(SUCCEEDED(pFilter->EnumPins(&pEnumPins)))
		{
			while(SUCCEEDED(pEnumPins->Next(1, &pPin, &tFetchPinCount)))
			{
				if(tFetchPinCount == 0)
					break;

				IPin* pAnother = NULL;

				pPin->ConnectedTo(&pAnother);

				if(pAnother)
				{
					bConnected = true;
					FCYSAFEKILL(pAnother);
					FCYSAFEKILL(pPin);
					break;
				}

				FCYSAFEKILL(pPin);
			}

			FCYSAFEKILL(pEnumPins);
		}

		if(!bConnected)
			tFilterToRemove.push(pFilter);

		FCYSAFEKILL(pFilter);
	}

	FCYSAFEKILL(pEnum);

	while(!tFilterToRemove.empty())
	{
		m_pFilterGraph->RemoveFilter(tFilterToRemove.top());
		tFilterToRemove.pop();
	}
}

f2dVideoRendererListener* f2dVideoImpl::GetRendererListener()
{
	return m_pRenderer->GetRendererListener();
}

void f2dVideoImpl::SetRendererListener(f2dVideoRendererListener* pListener)
{
	m_pRenderer->SetRendererListener(pListener);
}

fResult f2dVideoImpl::Update()
{
	if(m_pTex)
		m_pRenderer->CopyDataToTexture(m_pTex);

	// 更新状态
	long tEvtCode = 0;
	LONG_PTR tEvtParam1 = NULL;
	LONG_PTR tEvtParam2 = NULL;
	while(SUCCEEDED(m_pMediaEvent->GetEvent(&tEvtCode, &tEvtParam1, &tEvtParam2, 0)))
	{
		m_pMediaEvent->FreeEventParams(tEvtCode, tEvtParam1, tEvtParam2);

		switch (tEvtCode)
		{
		case EC_COMPLETE:
			if(m_bLoop)
			{
				LONGLONG tCur = 0;
				m_pMediaSeek->SetPositions(&tCur, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
			}
			else
			{
				LONGLONG tCur = 0;
				m_pMediaSeek->SetPositions(&tCur, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
				m_pMediaControl->StopWhenReady();
			}
			break;
		};
	};

	return FCYERR_OK;
}

f2dTexture* f2dVideoImpl::GetVideoTex()
{
	return m_pTex;
}

fuInt f2dVideoImpl::GetVideoWidth()
{
	return m_pRenderer->GetVideoWidth();
}

fuInt f2dVideoImpl::GetVideoHeight()
{
	return m_pRenderer->GetVideoHeight();
}

fFloat f2dVideoImpl::GetVideoDataRate()
{
	return m_pRenderer->GetDataPerSecond();
}

fFloat f2dVideoImpl::GetVideoRenderFPS()
{
	return m_pRenderer->GetVideoRenderFPS();
}

fResult f2dVideoImpl::CopyDataToTexture(f2dTexture* pTex)
{
	return m_pRenderer->CopyDataToTexture(pTex);
}

fBool f2dVideoImpl::IsLoop()
{
	return m_bLoop;
}

void f2dVideoImpl::SetLoop(fBool bLoop)
{
	m_bLoop = bLoop;
}

F2DVIDEOSTATE f2dVideoImpl::GetState()
{
	F2DVIDEOSTATE tRet = F2DVIDEOSTATE_STOPPED;
	HRESULT tHR = m_pMediaControl->GetState(0, (OAFilterState*)&tRet);
	return tRet;
}

fResult f2dVideoImpl::Play()
{
	return SUCCEEDED(m_pMediaControl->Run())?FCYERR_OK : FCYERR_INTERNALERR;
}

fResult f2dVideoImpl::Pause()
{
	return SUCCEEDED(m_pMediaControl->Pause())?FCYERR_OK : FCYERR_INTERNALERR;
}

fResult f2dVideoImpl::Stop()
{
	LONGLONG tCur = 0;
	m_pMediaSeek->SetPositions(&tCur, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

	return SUCCEEDED(m_pMediaControl->StopWhenReady())?FCYERR_OK : FCYERR_INTERNALERR;
}

fDouble f2dVideoImpl::GetTotalTime()
{
	LONGLONG tTime = 0;

	m_pMediaSeek->GetDuration(&tTime);

	return tTime/1000./10000.;
}

fDouble f2dVideoImpl::GetCurTime()
{
	LONGLONG tTime = 0;

	m_pMediaSeek->GetPositions(&tTime, NULL);

	return tTime/1000./10000.;
}

fResult f2dVideoImpl::SetCurTime(fDouble Time)
{
	LONGLONG tTime = (LONGLONG)(Time * 1000 * 10000);

	if(FAILED(m_pMediaSeek->SetPositions(&tTime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning)))
		return FCYERR_INTERNALERR;
	else
	{
		if(GetState() == F2DVIDEOSTATE_STOPPED)
			m_pMediaControl->StopWhenReady();
		else if(GetState() == F2DVIDEOSTATE_PAUSED)
		{
			m_pMediaControl->StopWhenReady();
			m_pMediaControl->Pause();
		}

		return FCYERR_OK;
	}
}

fResult f2dVideoImpl::Seek(fuInt KeyFrame)
{
	LONGLONG tTime = KeyFrame;

	m_pMediaSeek->SetTimeFormat(&TIME_FORMAT_FRAME);

	if(FAILED(m_pMediaSeek->SetPositions(&tTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning)))
	{
		m_pMediaSeek->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
		return FCYERR_INTERNALERR;
	}
	else
	{
		if(GetState() == F2DVIDEOSTATE_STOPPED)
			m_pMediaControl->StopWhenReady();
		else if(GetState() == F2DVIDEOSTATE_PAUSED)
		{
			m_pMediaControl->StopWhenReady();
			m_pMediaControl->Pause();
		}
	}

	m_pMediaSeek->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);

	return FCYERR_OK;
}

fDouble f2dVideoImpl::GetRate()
{
	double tRet = 1.;
	m_pMediaSeek->GetRate(&tRet);
	return tRet;
}

fResult f2dVideoImpl::SetRate(fDouble Rate)
{
	if(Rate < 0.)
		Rate = 0.;
	
	if(FCYFAILED(m_pMediaSeek->SetRate(Rate)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fFloat f2dVideoImpl::GetVolume()
{
	long pRet = 0;
	m_pAudioControl->get_Volume(&pRet);
	return (pRet + 10000) / 10000.f;
}

fResult f2dVideoImpl::SetVolume(fFloat Volume)
{
	if(Volume > 1.f)
		Volume = 1.f;
	else if(Volume < 0.f)
		Volume = 0.f;
	if(FCYFAILED(m_pAudioControl->put_Volume((long)((Volume - 1.f) * 10000.f))))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fFloat f2dVideoImpl::GetPan()
{
	LONG tRet = 0;
	m_pAudioControl->get_Balance(&tRet);
	return tRet/10000.f;
}

fResult f2dVideoImpl::SetPan(fFloat Pan)
{
	if(Pan < -1.f)
		Pan = -1.f;
	else if(Pan > 1.f)
		Pan = 1.f;
	
	if(FAILED(m_pAudioControl->put_Balance((long)(Pan * 10000))))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dVideoSysImpl::f2dVideoSysImpl(f2dEngineImpl* pEngine)
	: m_pEngine(pEngine), m_pDev(pEngine->GetRenderer()->GetDevice())
{
	HRESULT tHR = CoInitializeEx( NULL, COINIT_MULTITHREADED );
}

f2dVideoSysImpl::~f2dVideoSysImpl()
{}

GUID f2dVideoSysImpl::strToGUID(fcStrW pGUIDStr)
{
	GUID tRet = { 0 };
	int tData[8] = { 0 };

	swscanf_s(pGUIDStr, L"%08x-%04x-%04x-%02x%02x-%2x%2x%2x%2x%2x%2x", 
		&tRet.Data1, &tRet.Data2, &tRet.Data3,
		&tData[0], &tData[1],
		&tData[2], &tData[3],
		&tData[4], &tData[5],
		&tData[6], &tData[7]);

	for(int i = 0; i<8; i++)
		tRet.Data4[i] = tData[i];

	return tRet;
}

BOOL f2dVideoSysImpl::createInstanceFromDll(HINSTANCE hDll, REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
    typedef HRESULT (__stdcall *GETCLASS_PROC)(REFCLSID,REFIID,LPVOID*);
    
    GETCLASS_PROC procGetClassObject = (GETCLASS_PROC)GetProcAddress( hDll, "DllGetClassObject" ); 
    
    if( procGetClassObject )
    {
		IClassFactory* pFactory = NULL;
        
        HRESULT hr = procGetClassObject(rclsid, IID_IClassFactory, (void**)&pFactory);
		if(pFactory)
		{
			hr = pFactory->CreateInstance(NULL, riid, ppv);

			FCYSAFEKILL(pFactory);

			return *ppv!=NULL;
		}
    }
    
    return FALSE;
}

f2dRenderDevice* f2dVideoSysImpl::GetRenderDevice()
{
	return m_pDev;
}

fResult f2dVideoSysImpl::LoadFilter(fcStrW pGUIDStr)
{
	IBaseFilter* pFilter = NULL;
	GUID tGUID = strToGUID(pGUIDStr);

	HRESULT tHR = CoCreateInstance(tGUID, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFilter);
	if(FAILED(tHR))
	{
		m_pEngine->ThrowException(fcyException("f2dVideoSysImpl::LoadFilter", "CoCreateInstance Failed."));
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pFilter);

	for(fuInt i = 0; i<m_FliterGUIDList.size(); i++)
		if(m_FliterGUIDList[i].FilterGUID == tGUID)
			return FCYERR_OBJEXSITED;

	f2dVideoFilterInfo tInfo = { NULL, tGUID };
	m_FliterGUIDList.push_back(tInfo);

	return FCYERR_OK;
}

fResult f2dVideoSysImpl::LoadFilterFromAX(fcStrW AXFilePath, fcStrW pGUIDStr)
{
	IBaseFilter* pFilter = NULL;
	GUID tGUID = strToGUID(pGUIDStr);

	HMODULE tDLL = LoadLibraryEx(AXFilePath, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

	if(FALSE == createInstanceFromDll(tDLL, tGUID, IID_IBaseFilter, (void**)&pFilter))
	{
		m_pEngine->ThrowException(fcyException("f2dVideoSysImpl::LoadFilterFromAX", "createInstanceFromDll Failed."));
		return FCYERR_INTERNALERR;
	}

	FCYSAFEKILL(pFilter);

	for(fuInt i = 0; i<m_FliterGUIDList.size(); i++)
		if(m_FliterGUIDList[i].FilterGUID == tGUID)
			return FCYERR_OBJEXSITED;

	f2dVideoFilterInfo tInfo = { tDLL, tGUID };
	m_FliterGUIDList.push_back(tInfo);

	return FCYERR_OK;
}

fResult f2dVideoSysImpl::CreateVideo(f2dStream* pStream, fuInt Flag, f2dVideo** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pStream)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dVideoImpl(this, pStream, Flag);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}
