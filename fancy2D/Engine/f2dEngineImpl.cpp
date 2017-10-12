#include "f2dEngineImpl.h"

#include "../Input/f2dInputSysImpl.h"
#include "../Sound/f2dSoundSysImpl.h"
#include "../Video/f2dVideoSysImpl.h"
#include "../Renderer/f2dRendererImpl.h"

#include "fcyOS/fcyDebug.h"
#include "fcyOS/fcyCPUID.h"

#define F2DEXPORT
#include "../f2d.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// ��������
extern "C" fResult F2DDLLFUNC CreateF2DEngine(fuInt Version, f2dEngineEventListener* pListener, f2dEngine** pOut, f2dInitialErrListener* pErrListener)
{
	if(!pOut)
		return FCYERR_ILLEGAL;
	*pOut = NULL;

	if(Version != F2DVERSION)
		return FCYERR_INVAILDVERSION;
	
	try
	{
		*pOut = new f2dEngineImpl(pListener);
	}
	catch(const fcyException& e)
	{
		if(pErrListener)
			pErrListener->OnErr(e.GetTime(), e.GetSrc(), e.GetDesc());

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

extern "C" fResult F2DDLLFUNC CreateF2DEngineAndInit(fuInt Version, const fcyRect& WinPos, fcStrW Title, fBool Windowed, fBool VSync, F2DAALEVEL AA, f2dEngineEventListener* pListener, f2dEngine** pOut, f2dInitialErrListener* pErrListener)
{
	if(!pOut)
		return FCYERR_ILLEGAL;
	*pOut = NULL;

	if(Version != F2DVERSION)
		return FCYERR_INVAILDVERSION;
	
	try
	{
		*pOut = new f2dEngineImpl(WinPos, Title, Windowed, VSync, AA, pListener);
	}
	catch(const fcyException& e)
	{
		if(pErrListener)
			pErrListener->OnErr(e.GetTime(), e.GetSrc(), e.GetDesc());

		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

fuInt f2dEngineImpl::UpdateAndRenderThread::ThreadJob()
{
	// �߳��������
	SetThreadAffinityMask(GetCurrentThread(), 1);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// ��ʼ����������FPS������
	fcyStopWatch tTimer;
	f2dFPSControllerImpl tFPSController(m_MaxFPS);

	// �����Ⱦ�豸
	f2dRenderDeviceImpl* tpRenderDev = NULL;
	if(m_pEngine->GetRenderer())
		tpRenderDev = (f2dRenderDeviceImpl*)m_pEngine->GetRenderer()->GetDevice();

	// ��ʼ��ʱ
	tTimer.Reset();

	// ִ����Ⱦ����ѭ��
	fcyCriticalSection& tLock = m_pEngine->m_Sec;
	fBool bExit = false;
	fBool bDoPresent = false;
	fDouble tTime = 0;
	while(1)
	{
		tLock.Lock();
		bExit = m_pEngine->m_bStop;
		tLock.UnLock();

		// ����˳�
		if(bExit)
			break;
		
		// ����FPS
		tTime = tFPSController.Update(tTimer);
		
		// ִ����ʾ�¼�
		if(bDoPresent)
			m_pEngine->DoPresent(tpRenderDev);

		// ִ�и����¼�
		m_pEngine->DoUpdate(tTime, &tFPSController);

		// ִ����Ⱦ�¼�
		bDoPresent = m_pEngine->DoRender(tTime, &tFPSController, tpRenderDev);
	}

	// Ͷ����ֹ��Ϣ
	PostThreadMessage(m_MainThreadID, WM_USER, 0, 0);

	return 0;
}

fuInt f2dEngineImpl::UpdateThread::ThreadJob()
{
	// ��ʼ����������FPS������
	fcyStopWatch tTimer;
	f2dFPSControllerImpl tFPSController(m_MaxFPS);

	// ��ʼ��ʱ
	tTimer.Reset();

	// ִ����Ⱦ����ѭ��
	fcyCriticalSection& tLock = m_pEngine->m_Sec;
	fBool bExit = false;
	fDouble tTime = 0;
	while(1)
	{
		tLock.Lock();
		bExit = m_pEngine->m_bStop;
		tLock.UnLock();

		// ����˳�
		if(bExit)
			break;

		// ����FPS
		tTime = tFPSController.Update(tTimer);
		
		// ִ�и����¼�
		m_pEngine->DoUpdate(tTime, &tFPSController);
	}

	// Ͷ����ֹ��Ϣ
	PostThreadMessage(m_MainThreadID, WM_USER, 0, 0);

	return 0;
}

fuInt f2dEngineImpl::RenderThread::ThreadJob()
{
	// ��ʼ����������FPS������
	fcyStopWatch tTimer;
	f2dFPSControllerImpl tFPSController(m_MaxFPS);

	// �����Ⱦ�豸
	f2dRenderDeviceImpl* tpRenderDev = NULL;
	if(m_pEngine->GetRenderer())
		tpRenderDev = (f2dRenderDeviceImpl*)m_pEngine->GetRenderer()->GetDevice();

	// ��ʼ��ʱ
	tTimer.Reset();

	// ִ����Ⱦ����ѭ��
	fcyCriticalSection& tLock = m_pEngine->m_Sec;
	fBool bExit = false;
	fBool bDoPresent = false;
	fDouble tTime = 0;
	while(1)
	{
		tLock.Lock();
		bExit = m_pEngine->m_bStop;
		tLock.UnLock();

		// ����˳�
		if(bExit)
			break;

		// ����FPS
		tTime = tFPSController.Update(tTimer);

		// ִ����ʾ�¼�
		if(bDoPresent)
			m_pEngine->DoPresent(tpRenderDev);
		
		// ִ����Ⱦ�¼�
		bDoPresent = m_pEngine->DoRender(tTime, &tFPSController, tpRenderDev);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

f2dEngineImpl::f2dEngineImpl(f2dEngineEventListener* pListener)
	: m_bStop(true), m_pListener(pListener), m_LastErrTime(0), m_PumpIndex(0),
	m_MainThreadID(GetCurrentThreadId()), m_ThreadMode(F2DENGTHREADMODE_SINGLETHREAD),
	m_WinClass(this, L"F2DRenderWindow"),
	m_pWindow(NULL), m_pSoundSys(NULL), m_pInputSys(NULL), m_pRenderer(NULL), m_pVideoSys(NULL)
{
	m_CPUString = fcyCPUID::GetCPUString();
	m_CPUBrandString = fcyCPUID::GetCPUBrand();
}

f2dEngineImpl::f2dEngineImpl(const fcyRect& WinPos, fcStrW Title, fBool Windowed, fBool VSync, F2DAALEVEL AA, f2dEngineEventListener* pListener)
	: m_bStop(true), m_pListener(pListener), m_LastErrTime(0), m_PumpIndex(0),
	m_MainThreadID(GetCurrentThreadId()), m_ThreadMode(F2DENGTHREADMODE_SINGLETHREAD),
	m_WinClass(this, L"F2DRenderWindow"),
	m_pWindow(NULL), m_pSoundSys(NULL), m_pInputSys(NULL), m_pRenderer(NULL), m_pVideoSys(NULL)
{
	m_CPUString = fcyCPUID::GetCPUString();
	m_CPUBrandString = fcyCPUID::GetCPUBrand();

	// ��ʼ������
	try
	{
		m_pWindow = m_WinClass.CreateRenderWindow(WinPos, Title, false, F2DWINBORDERTYPE_FIXED);
		m_pRenderer = new f2dRendererImpl(this, (fuInt)WinPos.GetWidth(), (fuInt)WinPos.GetHeight(), Windowed, VSync, AA);
		m_pSoundSys = new f2dSoundSysImpl(this);
#ifndef _M_ARM
		m_pInputSys = new f2dInputSysImpl(this);
		m_pVideoSys = new f2dVideoSysImpl(this);
#endif
	}
	catch(...)
	{
		FCYSAFEKILL(m_pVideoSys);
		FCYSAFEKILL(m_pRenderer);
		FCYSAFEKILL(m_pInputSys);
		FCYSAFEKILL(m_pSoundSys);
		FCYSAFEKILL(m_pWindow);

		throw;
	}
}

f2dEngineImpl::~f2dEngineImpl()
{
	// �������
	FCYSAFEKILL(m_pVideoSys);
	FCYSAFEKILL(m_pRenderer);
	FCYSAFEKILL(m_pInputSys);
	FCYSAFEKILL(m_pSoundSys);
	FCYSAFEKILL(m_pWindow);
}

void f2dEngineImpl::ThrowException(const fcyException& e)
{
	FCYDEBUGEXCPT(e);

	// �������һ�δ���
	m_LastErrTime = e.GetTime();
	m_LastErrSrc = e.GetSrc();
	m_LastErrDesc = e.GetDesc();

	// ��װ���׳���Ϣ
	f2dMsgMemHelper<fcyException>* tObjMem = new f2dMsgMemHelper<fcyException>(e);
	SendMsg(
		F2DMSG_APP_ONEXCEPTION,
		e.GetTime(), 
		(fuLong)tObjMem->GetObj().GetSrc(),
		(fuLong)tObjMem->GetObj().GetDesc(),
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
}

fResult f2dEngineImpl::InitWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border)
{
	if(m_pWindow)
		return FCYERR_ILLEGAL;

	try
	{
		m_pWindow = m_WinClass.CreateRenderWindow(Pos, Title, false, F2DWINBORDERTYPE_FIXED);
	}
	catch(const fcyException& e)
	{
		ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dEngineImpl::InitSoundSys()
{
	if(m_pSoundSys || !m_pWindow)
		return FCYERR_ILLEGAL;

	try
	{
		m_pSoundSys = new f2dSoundSysImpl(this);
	}
	catch(const fcyException& e)
	{
		ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dEngineImpl::InitInputSys()
{
#ifndef _M_ARM
	if(m_pInputSys || !m_pWindow)
		return FCYERR_ILLEGAL;

	try
	{
		m_pInputSys = new f2dInputSysImpl(this);
	}
	catch(const fcyException& e)
	{
		ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
#else
	return FCYERR_NOTSUPPORT;
#endif
}

fResult f2dEngineImpl::InitRenderer(fuInt BufferWidth, fuInt BufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
{
	if(m_pRenderer || !m_pWindow)
		return FCYERR_ILLEGAL;

	try
	{
		m_pRenderer = new f2dRendererImpl(this, BufferWidth, BufferHeight, Windowed, VSync, AALevel);
	}
	catch(const fcyException& e)
	{
		ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dEngineImpl::InitVideoSys()
{
#ifndef _M_ARM
	if(m_pVideoSys || !m_pRenderer)
		return FCYERR_ILLEGAL;

	try
	{
		m_pVideoSys = new f2dVideoSysImpl(this);
	}
	catch(const fcyException& e)
	{
		ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
#else
	return FCYERR_NOTSUPPORT;
#endif
}

f2dSoundSys* f2dEngineImpl::GetSoundSys() { return m_pSoundSys; }
f2dInputSys* f2dEngineImpl::GetInputSys() { return m_pInputSys; }
f2dRenderer* f2dEngineImpl::GetRenderer() { return m_pRenderer; }
f2dVideoSys* f2dEngineImpl::GetVideoSys() { return m_pVideoSys; }

void f2dEngineImpl::Abort()
{
	m_Sec.Lock();
	m_bStop = true;
	m_Sec.UnLock();
}

void f2dEngineImpl::Run(F2DENGTHREADMODE ThreadMode, fuInt UpdateMaxFPS, fuInt RenderMaxFPS)
{
	m_Sec.Lock();
	m_bStop = false;
	m_Sec.UnLock();

	m_ThreadMode = ThreadMode;

	switch(ThreadMode)
	{
	case F2DENGTHREADMODE_SINGLETHREAD:
		Run_SingleThread(UpdateMaxFPS);
		break;
	case F2DENGTHREADMODE_MULTITHREAD:
		Run_MultiThread(UpdateMaxFPS);
		break;
	case F2DENGTHREADMODE_FULLMULTITHREAD:
		Run_FullMultiThread(UpdateMaxFPS, RenderMaxFPS);
		break;
	}
}

fResult f2dEngineImpl::SendMsg(const f2dMsg& Msg, f2dInterface* pMemObj)
{
	m_Sec.Lock();

	m_MsgPump[m_PumpIndex].Push(Msg, pMemObj);

	m_Sec.UnLock();

	return FCYERR_OK;
}

void f2dEngineImpl::Run_SingleThread(fuInt UpdateMaxFPS)
{
	// �߳��������
	SetThreadAffinityMask(GetCurrentThread(), 1);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// ��ʼ����������FPS������
	fcyStopWatch tTimer;
	fcyStopWatch tMsgTimer;
	f2dFPSControllerImpl tFPSController(UpdateMaxFPS);

	// �����Ⱦ�豸
	f2dRenderDeviceImpl* tpRenderDev = NULL;
	if(m_pRenderer)
		tpRenderDev = (f2dRenderDeviceImpl*)m_pRenderer->GetDevice();

	// ��ʼ��ʱ
	tTimer.Reset();

	// ִ�г���ѭ��
	fBool bExit = false;
	fBool bDoPresent = false;
	fDouble tTime = 0;
	fDouble tMsgTime = 0;
	MSG tMsg;
	while(1)
	{
		m_Sec.Lock();
		bExit = m_bStop;
		m_Sec.UnLock();

		if(bExit)
			break;

		// Ӧ�ó�����Ϣ����
		{
			tMsgTimer.Reset();
			if(PeekMessage(&tMsg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&tMsg);
				DispatchMessage(&tMsg);

				// �����˳���Ϣ
				if(tMsg.message == WM_QUIT)
					SendMsg(F2DMSG_APP_ONEXIT);
			}
			tMsgTime = tMsgTimer.GetElapsed();
		}

		// ����FPS
		tTime = tFPSController.Update(tTimer) - tMsgTime;  // �������ڴ�����Ϣ����ķѵ�ʱ��
		
		// ִ����ʾ�¼�
		if(bDoPresent)
			DoPresent(tpRenderDev);

		// ִ�и����¼�
		DoUpdate(tTime, &tFPSController);

		// ִ����Ⱦ�¼�
		bDoPresent = DoRender(tTime, &tFPSController, tpRenderDev);
	}
}

void f2dEngineImpl::Run_MultiThread(fuInt UpdateMaxFPS)
{
	// ���������̲߳�ִ��
	UpdateAndRenderThread tThread(this, UpdateMaxFPS);
	tThread.Resume();

	// ִ�г���ѭ��
	fBool bExit = false;
	MSG tMsg;
	while(1)
	{
		m_Sec.Lock();
		bExit = m_bStop;
		m_Sec.UnLock();

		if(bExit)
		{
			while(WAIT_TIMEOUT == WaitForSingleObject(tThread.GetHandle(), 10))
			{
				if(PeekMessage(&tMsg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&tMsg);
					DispatchMessage(&tMsg);
				}
			}
			
			break;
		}

		// Ӧ�ó�����Ϣ����
		if(GetMessage(&tMsg, 0, 0, 0))
		{
			TranslateMessage(&tMsg);
			DispatchMessage(&tMsg);
		}
		else
		{
			// �����˳���Ϣ
			if(tMsg.message == WM_QUIT)
				SendMsg(F2DMSG_APP_ONEXIT);
		}
	}

	// �ȴ������߳�
	tThread.Wait();
}

void f2dEngineImpl::Run_FullMultiThread(fuInt UpdateMaxFPS, fuInt RenderMaxFPS)
{
	// ���������̲߳�ִ��
	UpdateThread tUpdateThread(this, UpdateMaxFPS);
	RenderThread tRenderThread(this, RenderMaxFPS);
	tUpdateThread.Resume();
	tRenderThread.Resume();

	// ִ�г���ѭ��
	fBool bExit = false;
	MSG tMsg;
	while(1)
	{
		m_Sec.Lock();
		bExit = m_bStop;
		m_Sec.UnLock();

		if(bExit)
		{
			while(1)
			{
				if(WAIT_TIMEOUT != WaitForSingleObject(tUpdateThread.GetHandle(), 10))
					if(WAIT_TIMEOUT != WaitForSingleObject(tRenderThread.GetHandle(), 10))
						break;

				if(PeekMessage(&tMsg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&tMsg);
					DispatchMessage(&tMsg);
				}
			}
			
			break;
		}

		// Ӧ�ó�����Ϣ����
		if(GetMessage(&tMsg, 0, 0, 0))
		{
			TranslateMessage(&tMsg);
			DispatchMessage(&tMsg);
		}
		else
		{
			// �����˳���Ϣ
			if(tMsg.message == WM_QUIT)
				SendMsg(F2DMSG_APP_ONEXIT);
		}
	}

	// �ȴ������߳�
	tUpdateThread.Wait();
	tRenderThread.Wait();
}

void f2dEngineImpl::DoUpdate(fDouble ElapsedTime, f2dFPSControllerImpl* pFPSController)
{
	f2dMsgPump* tPump = NULL;

	// ������Ϣ�ò������Ϣ��
	m_Sec.Lock();
	tPump = &m_MsgPump[ m_PumpIndex ];
	m_PumpIndex = !m_PumpIndex;
	m_MsgPump[m_PumpIndex].Clear();
	m_Sec.UnLock();

	// ���������豸
	if(m_pInputSys)
		m_pInputSys->Update();

	// ִ�м�����
	if(m_pListener)
	{
		if(false == m_pListener->OnUpdate(ElapsedTime, pFPSController, tPump))
			Abort();
	}
}

bool f2dEngineImpl::DoRender(fDouble ElapsedTime, f2dFPSControllerImpl* pFPSController, f2dRenderDeviceImpl* pDev)
{
	// ͬ���豸״̬�������豸��ʧ
	if(pDev && FCYOK(pDev->SyncDevice()))
	{
		if(m_pListener) // ������Ⱦ�¼�
			return m_pListener->OnRender(ElapsedTime, pFPSController);
	}

	return false;
}

void f2dEngineImpl::DoPresent(f2dRenderDeviceImpl* pDev)
{
	// �ݽ�����
	pDev->Present();
}
