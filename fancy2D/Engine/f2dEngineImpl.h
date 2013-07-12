////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEngineImpl.h
/// @brief fancy2D����ӿ�ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyIO/fcyStream.h>
#include <fcyOS/fcyMultiThread.h>

#include "f2dFPSControllerImpl.h"
#include "f2dMsgPumpImpl.h"

#include "f2dFileSysImpl.h"
#include "f2dWindowImpl.h"

class f2dWindowImpl;
class f2dSoundSysImpl;
class f2dInputSysImpl;
class f2dRendererImpl;
class f2dRenderDeviceImpl;
class f2dVideoSysImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D����ӿ�ʵ��
////////////////////////////////////////////////////////////////////////////////
class f2dEngineImpl :
	public fcyRefObjImpl<f2dEngine>
{
private:
	// �߼���Ⱦ�߳�
	class UpdateAndRenderThread :
		public fcyBaseThread
	{
	private:
		fuInt m_MaxFPS;
		DWORD m_MainThreadID;
		f2dEngineImpl* m_pEngine;
	protected:
		fuInt ThreadJob();
	public:
		UpdateAndRenderThread(f2dEngineImpl* pEngine, fuInt MaxFPS)
			: m_MaxFPS(MaxFPS), m_MainThreadID(GetCurrentThreadId()), m_pEngine(pEngine) {}
	};

	// �߼��߳�
	class UpdateThread :
		public fcyBaseThread
	{
	private:
		fuInt m_MaxFPS;
		DWORD m_MainThreadID;
		f2dEngineImpl* m_pEngine;
	protected:
		fuInt ThreadJob();
	public:
		UpdateThread(f2dEngineImpl* pEngine, fuInt MaxFPS)
			: m_MaxFPS(MaxFPS), m_MainThreadID(GetCurrentThreadId()), m_pEngine(pEngine) {}
	};

	// ��Ⱦ�߳�
	class RenderThread :
		public fcyBaseThread
	{
	private:
		fuInt m_MaxFPS;
		DWORD m_MainThreadID;
		f2dEngineImpl* m_pEngine;
	protected:
		fuInt ThreadJob();
	public:
		RenderThread(f2dEngineImpl* pEngine, fuInt MaxFPS)
			: m_MaxFPS(MaxFPS), m_MainThreadID(GetCurrentThreadId()), m_pEngine(pEngine) {}
	};
private:
	// ״̬
	bool m_bStop;

	// ������
	f2dEngineEventListener* m_pListener;

	// ��
	fcyCriticalSection m_Sec;
	DWORD m_MainThreadID;
	F2DENGTHREADMODE m_ThreadMode;

	// CPU��Ϣ
	std::string m_CPUString;
	std::string m_CPUBrandString;

	// ��һ�δ���
	fuInt m_LastErrTime;
	std::string m_LastErrSrc;
	std::string m_LastErrDesc;

	// �����������
	f2dWindowClass m_WinClass;
	f2dFileSysImpl m_FileSys;

	// ���
	f2dWindowImpl*   m_pWindow;
	f2dSoundSysImpl* m_pSoundSys;
	f2dInputSysImpl* m_pInputSys;
	f2dRendererImpl* m_pRenderer;
	f2dVideoSysImpl* m_pVideoSys;

	// ��Ϣ��
	fuInt m_PumpIndex;
	f2dMsgPumpImpl m_MsgPump[2];
protected:
	void Run_SingleThread(fuInt UpdateMaxFPS);
	void Run_MultiThread(fuInt UpdateMaxFPS);
	void Run_FullMultiThread(fuInt UpdateMaxFPS, fuInt RenderMaxFPS);

	void DoUpdate(fDouble ElapsedTime, f2dFPSControllerImpl* pFPSController);
	void DoRender(fDouble ElapsedTime, f2dFPSControllerImpl* pFPSController, f2dRenderDeviceImpl* pDev);
public: // �ڲ���������
	// ���쳣������ʱ���øú���
	void ThrowException(const fcyException& e);
public: // �ӿ�ʵ��
	f2dEngineEventListener* GetListener()                  { return m_pListener; }
	fResult SetListener(f2dEngineEventListener* pListener) { m_pListener = pListener; return FCYERR_OK; }

	fResult InitWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border);
	fResult InitSoundSys();
	fResult InitInputSys();
	fResult InitRenderer(fuInt BufferWidth, fuInt BufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel);
	fResult InitVideoSys();

	f2dWindow* GetMainWindow() { return m_pWindow;   }
	f2dFileSys* GetFileSys()   { return &m_FileSys;  }
	f2dSoundSys* GetSoundSys();
	f2dInputSys* GetInputSys();
	f2dRenderer* GetRenderer();
	f2dVideoSys* GetVideoSys();

	void ClearLstErr()
	{
		m_LastErrTime = 0;
		m_LastErrSrc = m_LastErrDesc = "";
	}
	fuInt GetLastErrTime() { return m_LastErrTime;         }
	fcStr GetLastErrSrc()  { return m_LastErrSrc.c_str();  }
	fcStr GetLastErrDesc() { return m_LastErrDesc.c_str(); }

	void Abort();
	void Run(F2DENGTHREADMODE ThreadMode, fuInt UpdateMaxFPS, fuInt RenderMaxFPS);
	F2DENGTHREADMODE GetCurrentThreadMode() { return m_ThreadMode; }

	fResult SendMsg(const f2dMsg& Msg, f2dInterface* pMemObj = NULL);
	fResult SendMsg(F2DMSGTYPE Type, fuLong Param1 = 0, fuLong Param2 = 0, fuLong Param3 = 0, fuLong Param4 = 0, f2dInterface* pMemObj = NULL)
	{
		f2dMsg tMsg = { Type, Param1, Param2, Param3, Param4 };
		return SendMsg(tMsg, pMemObj);
	}

	fResult InvokeDelegate(f2dMainThreadDelegate* pDelegate)
	{
		if(!pDelegate || !m_pWindow)
			return FCYERR_INVAILDPARAM;

		pDelegate->AddRef();
		if(TRUE == PostMessage((HWND)m_pWindow->GetHandle(), WM_USER, 0, (LPARAM)pDelegate))
			return FCYERR_OK;
		else
			return FCYERR_INTERNALERR;
	}
	fResult InvokeDelegateAndWait(f2dMainThreadDelegate* pDelegate)
	{
		if(!pDelegate || !m_pWindow)
			return FCYERR_INVAILDPARAM;

		pDelegate->AddRef();
		if(TRUE == SendMessage((HWND)m_pWindow->GetHandle(), WM_USER, 0, (LPARAM)pDelegate))
			return FCYERR_OK;
		else
			return FCYERR_INTERNALERR;
	}

	void GetCPUInfo(f2dCPUInfo& tInfo)
	{
		tInfo.CPUString = m_CPUString.c_str();
		tInfo.CPUBrandString = m_CPUBrandString.c_str();
	}
public:
	f2dEngineImpl(f2dEngineEventListener* pListener);
	f2dEngineImpl(const fcyRect& WinPos, fcStrW Title, fBool Windowed, fBool VSync, F2DAALEVEL AA, f2dEngineEventListener* pListener);
	~f2dEngineImpl();
};