#include "f2dSoundSysImpl.h"

#include "f2dEngineImpl.h"

#include "fcyException.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

f2dSoundSysAPI::f2dSoundSysAPI()
	: m_hModule(NULL),
	m_Entry_pDirectSoundCreate8(NULL)
{
	// 加载模块
	m_hModule = LoadLibrary(L"DSound.dll");
	if(!m_hModule)
		throw fcyWin32Exception("f2dSoundSysAPI::f2dSoundSysAPI", "LoadLibrary(DSound.dll) Failed.");

	// 获得入口
	m_Entry_pDirectSoundCreate8 = (pDLLEntry_DirectSoundCreate8)GetProcAddress(m_hModule, "DirectSoundCreate8");
	if(!m_Entry_pDirectSoundCreate8)
		throw fcyWin32Exception("f2dSoundSysAPI::f2dSoundSysAPI", "GetProcAddress(DirectSoundCreate8) Failed.");
}

f2dSoundSysAPI::~f2dSoundSysAPI()
{
}

HRESULT f2dSoundSysAPI::DLLEntry_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	return m_Entry_pDirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
}

////////////////////////////////////////////////////////////////////////////////

f2dSoundBufferStatic::f2dSoundBufferStatic(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus)
	: m_bLoop(false), m_pBuffer(NULL)
{
	if(!pDecoder)
		throw fcyException("f2dSoundBufferStatic::f2dSoundBufferStatic", "Invalid Pointer.");

	// 计算常量
	m_BufferSize = pDecoder->GetBufferSize();
	m_psSize = pDecoder->GetSamplesPerSec() * pDecoder->GetBlockAlign();

	WAVEFORMATEX tFmtDesc;
	memset(&tFmtDesc, 0, sizeof(tFmtDesc));
	tFmtDesc.nAvgBytesPerSec = pDecoder->GetAvgBytesPerSec();
	tFmtDesc.nBlockAlign = pDecoder->GetBlockAlign();
	tFmtDesc.nChannels = pDecoder->GetChannelCount();
	tFmtDesc.nSamplesPerSec = pDecoder->GetSamplesPerSec();
	tFmtDesc.wFormatTag = pDecoder->GetFormatTag();
	tFmtDesc.wBitsPerSample = pDecoder->GetBitsPerSample();
	tFmtDesc.cbSize = sizeof(tFmtDesc);

	DSBUFFERDESC tDesc;
	memset(&tDesc, 0, sizeof(DSBUFFERDESC)); 
	tDesc.dwSize = sizeof(DSBUFFERDESC); 
	tDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if(bGlobalFocus)
		tDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	tDesc.dwBufferBytes = m_BufferSize;  // 静态缓冲
	tDesc.lpwfxFormat = &tFmtDesc;
	
	HRESULT tHR = pSound->CreateSoundBuffer(&tDesc, &m_pBuffer, NULL);
	if(FAILED(tHR))
		throw fcyException("f2dSoundBufferStatic::f2dSoundBufferStatic", "CreateSoundBuffer Failed.");

	// 拷贝数据
	void* pBufferData = NULL;
	DWORD pBufferSize = 0;
	tHR = m_pBuffer->Lock(0, pDecoder->GetBufferSize(), &pBufferData, &pBufferSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if(FCYFAILED(pDecoder->Read((fByte*)pBufferData, pBufferSize, NULL)))
	{
		m_pBuffer->Unlock(pBufferData, pBufferSize, NULL, 0);
		FCYSAFEKILL(m_pBuffer);
		throw fcyException("f2dSoundBufferStatic::f2dSoundBufferStatic", "f2dSoundDecoder::Read Failed.");
	}
	m_pBuffer->Unlock(pBufferData, pBufferSize, NULL, 0);
}

f2dSoundBufferStatic::~f2dSoundBufferStatic()
{
	FCYSAFEKILL(m_pBuffer);
}

fBool f2dSoundBufferStatic::IsDynamic()
{
	return false;
}

void f2dSoundBufferStatic::Play()
{
	m_pBuffer->Play(0, 0, m_bLoop?DSBPLAY_LOOPING:0);
}

void f2dSoundBufferStatic::Stop()
{
	m_pBuffer->Stop();
	m_pBuffer->SetCurrentPosition(0);
}

void f2dSoundBufferStatic::Pause()
{
	m_pBuffer->Stop();
}

fBool f2dSoundBufferStatic::IsLoop()
{
	return m_bLoop;
}

void f2dSoundBufferStatic::SetLoop(fBool bValue)
{
	m_bLoop = bValue;
	if(IsPlaying())
	{
		Pause();
		Play();
	}
}

fBool f2dSoundBufferStatic::IsPlaying()
{
	DWORD tV = 0;
	m_pBuffer->GetStatus(&tV);
	return tV & DSBSTATUS_PLAYING;
}

fDouble f2dSoundBufferStatic::GetTotalTime()
{
	return m_BufferSize/(double)m_psSize;
}

fDouble f2dSoundBufferStatic::GetTime()
{
	DWORD tPlay;
	m_pBuffer->GetCurrentPosition(&tPlay, NULL);
	return tPlay/(double)m_psSize;
}

fResult f2dSoundBufferStatic::SetTime(fDouble Time)
{
	DWORD tPos = (DWORD)(Time * m_psSize);
	if(tPos>=m_BufferSize)
		return FCYERR_INVAILDPARAM;

	if(FAILED(m_pBuffer->SetCurrentPosition(tPos)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fFloat f2dSoundBufferStatic::GetVolume()
{
	LONG tRet = 0;
	m_pBuffer->GetVolume(&tRet);
	return (tRet - DSBVOLUME_MIN)/(float)(DSBVOLUME_MAX - DSBVOLUME_MIN);
}

fResult f2dSoundBufferStatic::SetVolume(fFloat Value)
{
	if(Value<0.f)
		Value = 0.f;
	else if(Value>1.f)
		Value = 1.f;

	LONG tValue = (LONG)(Value * (DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN);
	if(FAILED(m_pBuffer->SetVolume(tValue)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fFloat f2dSoundBufferStatic::GetPan()
{
	LONG tRet = 0;
	m_pBuffer->GetPan(&tRet);
	return (tRet)/(float)(DSBPAN_RIGHT);
}

fResult f2dSoundBufferStatic::SetPan(fFloat Value)
{
	if(Value<-1.f)
		Value = -1.f;
	else if(Value>1.f)
		Value = 1.f;

	LONG tValue = (LONG)(Value * DSBPAN_RIGHT);
	if(FAILED(m_pBuffer->SetPan(tValue)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

fuInt f2dSoundBufferStatic::GetFrequency()
{
	DWORD tRet = 0;
	m_pBuffer->GetFrequency(&tRet);
	return tRet;
}

fResult f2dSoundBufferStatic::SetFrequency(fuInt Value)
{
	if(FAILED(m_pBuffer->SetFrequency(Value)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

const fuInt f2dSoundBufferDynamic::BufferSize = 65536;

f2dSoundBufferDynamic::SoundBufferFiller::SoundBufferFiller(f2dSoundBufferDynamic* pParent)
	: m_pParent(pParent)
{}

f2dSoundBufferDynamic::SoundBufferFiller::~SoundBufferFiller()
{}

fuInt f2dSoundBufferDynamic::SoundBufferFiller::ThreadJob()
{
	bool tRunning = true;
	HANDLE tWaitList[3] = 
	{
		m_pParent->m_EvtBegin.GetHandle(),
		m_pParent->m_EvtHalf.GetHandle(),
		m_pParent->m_EvtStop.GetHandle()
	};

	while(tRunning)
	{
		DWORD tRet = WaitForMultipleObjects(3, tWaitList, FALSE, -1);
		switch(tRet)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			break;
		default:
			{
				// 检查线程退出
				if(m_pParent->isThreadHalt())
					return 0;

				fuInt nIndex = tRet - WAIT_OBJECT_0;
				switch(nIndex)
				{
				case 0:
					m_pParent->m_Sec.Lock();
					m_pParent->fillBuffer(1);
					m_pParent->updateTime();
					m_pParent->m_EvtBegin.Reset();
					m_pParent->m_Sec.UnLock();
					break;
				case 1:
					m_pParent->m_Sec.Lock();
					m_pParent->fillBuffer(0);
					m_pParent->m_EvtHalf.Reset();
					m_pParent->m_Sec.UnLock();
					break;
				case 2:
					{
						m_pParent->Stop();
						if(m_pParent->IsLoop())
						{
							m_pParent->preInit(0);
							m_pParent->Play();
						}
						m_pParent->m_EvtStop.Reset();
					}
					break;
				}
			}
		};
	}
	return 0;
}

f2dSoundBufferDynamic::f2dSoundBufferDynamic(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus)
	: m_pBuffer(NULL), m_pNotify(NULL), m_pDecoder(pDecoder), 
	m_pFiller(NULL), m_bHalt(false),
	m_StartPos(0), m_PlayTime(0), m_bLoop(false), m_psSize(0), m_BufferSize(0)
{
	if(!m_pDecoder)
		throw fcyException("f2dSoundBufferDynamic::f2dSoundBufferDynamic", "Invalid Pointer.");
	m_pDecoder->AddRef();
	
	m_psSize = m_pDecoder->GetSamplesPerSec() * m_pDecoder->GetBlockAlign();
	m_BufferSize = m_pDecoder->GetBufferSize();

	WAVEFORMATEX tFmtDesc;
	memset(&tFmtDesc, 0, sizeof(tFmtDesc));
	tFmtDesc.nAvgBytesPerSec = pDecoder->GetAvgBytesPerSec();
	tFmtDesc.nBlockAlign = pDecoder->GetBlockAlign();
	tFmtDesc.nChannels = pDecoder->GetChannelCount();
	tFmtDesc.nSamplesPerSec = pDecoder->GetSamplesPerSec();
	tFmtDesc.wFormatTag = pDecoder->GetFormatTag();
	tFmtDesc.wBitsPerSample = pDecoder->GetBitsPerSample();
	tFmtDesc.cbSize = sizeof(tFmtDesc);

	DSBUFFERDESC tDesc;
	memset(&tDesc, 0, sizeof(DSBUFFERDESC)); 
	tDesc.dwSize = sizeof(DSBUFFERDESC); 
	tDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY;
	if(bGlobalFocus)
		tDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	tDesc.dwBufferBytes = BufferSize;  // 动态缓冲
	tDesc.lpwfxFormat = &tFmtDesc;
	
	HRESULT tHR = pSound->CreateSoundBuffer(&tDesc, &m_pBuffer, NULL);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pDecoder);
		throw fcyException("f2dSoundBufferDynamic::f2dSoundBufferDynamic", "CreateSoundBuffer Failed.");
	}

	tHR = m_pBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pNotify);
	if(FAILED(tHR))
	{
		FCYSAFEKILL(m_pBuffer);
		FCYSAFEKILL(m_pDecoder);
		throw fcyException("f2dSoundBufferDynamic::f2dSoundBufferDynamic", "QueryInterface Failed.");
	}

	preInit(0);

	// 启动监听线程
	m_pFiller = new SoundBufferFiller(this);
	m_pFiller->Resume();
}

f2dSoundBufferDynamic::~f2dSoundBufferDynamic()
{
	// 终止线程
	haltThread();

	FCYSAFEDEL(m_pFiller);
	FCYSAFEKILL(m_pDecoder);
	FCYSAFEKILL(m_pNotify);
	FCYSAFEKILL(m_pBuffer);
}

void f2dSoundBufferDynamic::preInit(fuInt StartPos)
{
	m_Sec.Lock();

	m_pBuffer->SetCurrentPosition(0);
	m_StartPos = StartPos;
	m_pDecoder->SetPosition(FCYSEEKORIGIN_BEG, StartPos);
	regNotify();   // 注册监听器
	fillBuffer(0); // 填充第一块

	m_PlayTime = 0 - BufferSize;

	m_Sec.UnLock();
}

void f2dSoundBufferDynamic::updateTime()
{
	m_PlayTime += BufferSize;
}

void f2dSoundBufferDynamic::fillBuffer(fuInt Index)
{
	// 计算填充位置
	fuInt tPosToFill = Index * (BufferSize / 2);
	
	// 获得指针
	fByte *tFillPtr = NULL;
	fuInt tFillSize = 0, tRealSize = 0;
	m_pBuffer->Lock(tPosToFill, BufferSize / 2, (void**)&tFillPtr, (DWORD*)&tFillSize, NULL, NULL, 0);
	
	memset(tFillPtr, 0, tFillSize);
	m_pDecoder->Read(tFillPtr, tFillSize, &tRealSize);
	if(tRealSize<tFillSize)
	{
		// 插入一个终止信号
		fuInt tStopPos = tPosToFill + tRealSize;

		regStopNotify(tStopPos);
	}

	m_pBuffer->Unlock(tFillPtr, tFillSize, NULL, 0);
}

void f2dSoundBufferDynamic::regNotify()
{
	DSBPOSITIONNOTIFY tNotifyList[2] = 
	{
		{ 0, m_EvtBegin.GetHandle() },
		{ BufferSize / 2, m_EvtHalf.GetHandle() }
	};

	m_pNotify->SetNotificationPositions(2, tNotifyList);
}

void f2dSoundBufferDynamic::regStopNotify(fuInt Pos)
{
	m_pBuffer->Stop();

	DSBPOSITIONNOTIFY tNotifyList[1] = 
	{
		{ Pos, m_EvtStop.GetHandle() }
	};

	m_pNotify->SetNotificationPositions(1, tNotifyList);

	m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

fBool f2dSoundBufferDynamic::isThreadHalt()
{
	bool tValue;

	m_Sec.Lock();
	tValue = m_bHalt;
	m_Sec.UnLock();

	return tValue;
}

void f2dSoundBufferDynamic::haltThread()
{
	m_Sec.Lock();
	m_bHalt = true;
	m_Sec.UnLock();

	// 激活所有事件以使线程退出
	m_EvtBegin.Set();
	m_EvtHalf.Set();
	m_EvtStop.Set();

	m_pFiller->Wait();
}

fBool f2dSoundBufferDynamic::IsDynamic()
{
	return true;
}

void f2dSoundBufferDynamic::Play()
{
	m_Sec.Lock();
	m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
	m_Sec.UnLock();
}

void f2dSoundBufferDynamic::Stop()
{
	m_Sec.Lock();
	m_pBuffer->Stop();
	preInit(0);
	m_Sec.UnLock();
}

void f2dSoundBufferDynamic::Pause()
{
	m_Sec.Lock();
	m_pBuffer->Stop();
	m_Sec.UnLock();
}

fBool f2dSoundBufferDynamic::IsLoop()
{
	fBool tRet;
	m_Sec.Lock();
	tRet = m_bLoop;
	m_Sec.UnLock();

	return tRet;
}

void f2dSoundBufferDynamic::SetLoop(fBool bValue)
{
	m_Sec.Lock();
	m_bLoop = bValue;
	m_Sec.UnLock();
}

fBool f2dSoundBufferDynamic::IsPlaying()
{
	DWORD tV = 0;
	m_Sec.Lock();
	m_pBuffer->GetStatus(&tV);
	m_Sec.UnLock();
	return tV & DSBSTATUS_PLAYING;
}

fDouble f2dSoundBufferDynamic::GetTotalTime()
{
	return m_BufferSize/(double)m_psSize;
}

fDouble f2dSoundBufferDynamic::GetTime()
{
	DWORD tPlay;

	m_Sec.Lock();
	m_pBuffer->GetCurrentPosition(&tPlay, NULL);
	tPlay += m_StartPos;
	if(m_PlayTime >= 0)
		tPlay += m_PlayTime;
	m_Sec.UnLock();

	if(tPlay>m_BufferSize)
		tPlay = m_BufferSize;

	return tPlay/(double)m_psSize;
}

fResult f2dSoundBufferDynamic::SetTime(fDouble Time)
{
	fuInt tBytePos = (fuInt)(m_psSize * Time);
	
	if(tBytePos>m_BufferSize)
		return FCYERR_INVAILDPARAM;

	preInit(tBytePos);

	return FCYERR_OK;
}

fFloat f2dSoundBufferDynamic::GetVolume()
{
	LONG tRet = 0;

	m_Sec.Lock();
	m_pBuffer->GetVolume(&tRet);
	m_Sec.UnLock();

	return (tRet - DSBVOLUME_MIN)/(float)(DSBVOLUME_MAX - DSBVOLUME_MIN);
}

fResult f2dSoundBufferDynamic::SetVolume(fFloat Value)
{
	if(Value<0.f)
		Value = 0.f;
	else if(Value>1.f)
		Value = 1.f;

	LONG tValue = (LONG)(Value * (DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN);
	HRESULT tHR;
	m_Sec.Lock();
	tHR = m_pBuffer->SetVolume(tValue);
	m_Sec.UnLock();

	return (FAILED(tHR)?FCYERR_INTERNALERR : FCYERR_OK);
}

fFloat f2dSoundBufferDynamic::GetPan()
{
	LONG tRet = 0;

	m_Sec.Lock();
	m_pBuffer->GetPan(&tRet);
	m_Sec.UnLock();
	return (tRet)/(float)(DSBPAN_RIGHT);
}

fResult f2dSoundBufferDynamic::SetPan(fFloat Value)
{
	if(Value<-1.f)
		Value = -1.f;
	else if(Value>1.f)
		Value = 1.f;

	LONG tValue = (LONG)(Value * DSBPAN_RIGHT);
	HRESULT tHR;
	m_Sec.Lock();
	tHR = m_pBuffer->SetPan(tValue);
	m_Sec.UnLock();

	return (FAILED(tHR)?FCYERR_INTERNALERR : FCYERR_OK);
}

fuInt f2dSoundBufferDynamic::GetFrequency()
{
	DWORD tRet = 0;
	m_pBuffer->GetFrequency(&tRet);
	return tRet;
}

fResult f2dSoundBufferDynamic::SetFrequency(fuInt Value)
{
	if(FAILED(m_pBuffer->SetFrequency(Value)))
		return FCYERR_INTERNALERR;
	else
		return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dSoundSpriteImpl::f2dSoundSpriteImpl(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus)
	: m_pParent(pSound), m_pBuffer(NULL), m_BufferSize(0), m_psSize(0)
{
	m_pParent->AddRef();

	if(!pDecoder)
		throw fcyException("f2dSoundSpriteImpl::f2dSoundSpriteImpl", "Invalid Pointer.");

	// 计算常量
	m_BufferSize = pDecoder->GetBufferSize();
	m_psSize = pDecoder->GetSamplesPerSec() * pDecoder->GetBlockAlign();

	WAVEFORMATEX tFmtDesc;
	memset(&tFmtDesc, 0, sizeof(tFmtDesc));
	tFmtDesc.nAvgBytesPerSec = pDecoder->GetAvgBytesPerSec();
	tFmtDesc.nBlockAlign = pDecoder->GetBlockAlign();
	tFmtDesc.nChannels = pDecoder->GetChannelCount();
	tFmtDesc.nSamplesPerSec = pDecoder->GetSamplesPerSec();
	tFmtDesc.wFormatTag = pDecoder->GetFormatTag();
	tFmtDesc.wBitsPerSample = pDecoder->GetBitsPerSample();
	tFmtDesc.cbSize = sizeof(tFmtDesc);

	DSBUFFERDESC tDesc;
	memset(&tDesc, 0, sizeof(DSBUFFERDESC)); 
	tDesc.dwSize = sizeof(DSBUFFERDESC); 
	tDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if(bGlobalFocus)
		tDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	tDesc.dwBufferBytes = m_BufferSize;  // 静态缓冲
	tDesc.lpwfxFormat = &tFmtDesc;
	
	HRESULT tHR = pSound->CreateSoundBuffer(&tDesc, &m_pBuffer, NULL);
	if(FAILED(tHR))
		throw fcyException("f2dSoundSpriteImpl::f2dSoundSpriteImpl", "CreateSoundBuffer Failed.");

	// 拷贝数据
	void* pBufferData = NULL;
	DWORD pBufferSize = 0;
	tHR = m_pBuffer->Lock(0, pDecoder->GetBufferSize(), &pBufferData, &pBufferSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if(FCYFAILED(pDecoder->Read((fByte*)pBufferData, pBufferSize, NULL)))
	{
		m_pBuffer->Unlock(pBufferData, pBufferSize, NULL, 0);
		FCYSAFEKILL(m_pBuffer);
		throw fcyException("f2dSoundBufferStatic::f2dSoundBufferStatic", "f2dSoundDecoder::Read Failed.");
	}
	m_pBuffer->Unlock(pBufferData, pBufferSize, NULL, 0);
}

f2dSoundSpriteImpl::~f2dSoundSpriteImpl()
{
	deque<IDirectSoundBuffer*>::iterator i = m_BufferList.begin();
	while(i != m_BufferList.end())
	{
		IDirectSoundBuffer* pBuffer = *i;
		FCYSAFEKILL(pBuffer);
		i++;
	}

	FCYSAFEKILL(m_pBuffer);
	FCYSAFEKILL(m_pParent);
}

void f2dSoundSpriteImpl::Play()
{
	deque<IDirectSoundBuffer*>::iterator i = m_BufferList.begin();
	while(i != m_BufferList.end())
	{
		IDirectSoundBuffer* pBuffer = *i;
		DWORD tPos = 0;
		pBuffer->GetCurrentPosition(&tPos, NULL);
		if(tPos != 0)
			pBuffer->Play(0,0,0);
		i++;
	}
}

void f2dSoundSpriteImpl::Stop()
{
	deque<IDirectSoundBuffer*>::iterator i = m_BufferList.begin();
	while(i != m_BufferList.end())
	{
		IDirectSoundBuffer* pBuffer = *i;
		pBuffer->Stop();
		pBuffer->SetCurrentPosition(0);
		i++;
	}
}

void f2dSoundSpriteImpl::Pause()
{
	deque<IDirectSoundBuffer*>::iterator i = m_BufferList.begin();
	while(i != m_BufferList.end())
	{
		IDirectSoundBuffer* pBuffer = *i;
		pBuffer->Stop();
		i++;
	}
}

fDouble f2dSoundSpriteImpl::GetTotalTime()
{
	return m_BufferSize / (double)m_psSize;
}

fResult f2dSoundSpriteImpl::PlayNewSound(fFloat Volume, fFloat Pan)
{
	IDirectSoundBuffer* pBuffer = NULL;

	deque<IDirectSoundBuffer*>::iterator i = m_BufferList.begin();
	while(i != m_BufferList.end())
	{
		IDirectSoundBuffer* tBuffer = *i;
		DWORD tFlag = 0;
		DWORD tPos = 0;
		tBuffer->GetStatus(&tFlag);
		tBuffer->GetCurrentPosition(&tPos, NULL);
		if(!(tFlag & DSBSTATUS_PLAYING) && tPos==0)
		{
			pBuffer = tBuffer;
			break;
		}
		i++;
	}

	if(!pBuffer)
	{	
		HRESULT tHR = m_pParent->DuplicateSoundBuffer(m_pBuffer, &pBuffer);
		if(FAILED(tHR))
			return FCYERR_INTERNALERR;

		m_BufferList.push_back(pBuffer);
	}

	// 修饰值
	if(Volume<0.f)
		Volume = 0.f;
	else if(Volume>1.f)
		Volume = 1.f;
	if(Pan<-1.f)
		Pan = -1.f;
	else if(Pan>1.f)
		Pan = 1.f;

	LONG tVolume = (LONG)(Volume * (DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN);
	pBuffer->SetVolume(tVolume);

	LONG tPan = (LONG)(Pan * DSBPAN_RIGHT);
	pBuffer->SetPan(tPan);

	pBuffer->SetCurrentPosition(0);
	pBuffer->Play(0, 0, 0);

	return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

f2dSoundSysImpl::f2dSoundSysImpl(f2dEngineImpl* pEngine)
	: m_pEngine(pEngine)
{
	// 使用默认回放设备创建DSound
	HRESULT tHR;
	tHR = m_API.DLLEntry_DirectSoundCreate8(NULL, &m_pDSound8, NULL);
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dSoundSysImpl::f2dSoundSysImpl", "DirectSoundCreate8 Failed", tHR);

	// 设置设备协作性
	tHR = m_pDSound8->SetCooperativeLevel((HWND)pEngine->GetMainWindow()->GetHandle(), DSSCL_PRIORITY);
	if(FAILED(tHR))
		throw fcyWin32COMException("f2dSoundSysImpl::f2dSoundSysImpl", "SetCooperativeLevel Failed.", tHR);
}

f2dSoundSysImpl::~f2dSoundSysImpl()
{
	FCYSAFEKILL(m_pDSound8);
}

fResult f2dSoundSysImpl::CreateStaticBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut)
{
	if(!pDecoder || !pOut)
		return FCYERR_INVAILDPARAM;

	*pOut = NULL;
	f2dSoundBuffer* pBuffer = NULL;

	try
	{
		pBuffer = new f2dSoundBufferStatic(m_pDSound8, pDecoder, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pBuffer;
	return FCYERR_OK;
}

fResult f2dSoundSysImpl::CreateDynamicBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut)
{
	if(!pDecoder || !pOut)
		return FCYERR_INVAILDPARAM;

	*pOut = NULL;
	f2dSoundBuffer* pBuffer = NULL;

	try
	{
		pBuffer = new f2dSoundBufferDynamic(m_pDSound8, pDecoder, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pBuffer;
	return FCYERR_OK;
}

fResult f2dSoundSysImpl::CreateSoundSprite(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundSprite** pOut)
{
	if(!pDecoder || !pOut)
		return FCYERR_INVAILDPARAM;

	*pOut = NULL;
	f2dSoundSprite* pBuffer = NULL;

	try
	{
		pBuffer = new f2dSoundSpriteImpl(m_pDSound8, pDecoder, bGlobalFocus);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pBuffer;
	return FCYERR_OK;
}

fResult f2dSoundSysImpl::CreateWaveDecoder(f2dStream* pStream, f2dSoundDecoder** pOut)
{
	if(!pStream || !pOut)
		return FCYERR_INVAILDPARAM;

	*pOut = NULL;
	f2dSoundDecoder* pBuffer = NULL;

	try
	{
		pBuffer = new f2dWaveDecoder(pStream);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pBuffer;
	return FCYERR_OK;
}

fResult f2dSoundSysImpl::CreateOGGVorbisDecoder(f2dStream* pStream, f2dSoundDecoder** pOut)
{
	if(!pStream || !pOut)
		return FCYERR_INVAILDPARAM;

	*pOut = NULL;
	f2dSoundDecoder* pBuffer = NULL;

	try
	{
		pBuffer = new f2dVorbisDecoder(pStream);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	*pOut = pBuffer;
	return FCYERR_OK;
}
