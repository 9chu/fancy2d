////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEngineImpl.h
/// @brief fancy2D引擎接口实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyRefObj.h"
#include "fcyIO\fcyStream.h"

#include "../f2dEngine.h"

#include "f2dFileSysImpl.h"
#include "f2dWindowImpl.h"

class f2dWindowImpl;
class f2dSoundSysImpl;
class f2dInputSysImpl;
class f2dRendererImpl;
class f2dVideoSysImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D引擎接口实现
////////////////////////////////////////////////////////////////////////////////
class f2dEngineImpl :
	public fcyRefObjImpl<f2dEngine>
{
private:
	// 状态
	bool m_bStop;
	bool m_bPresent;
	float m_cFPS;
	fuInt m_iFPSMax;      // 最大FPS
	float m_FrameDelay;   // FPS控制选项

	// 监听器
	f2dEngineEventListener* m_pListener;

	// 长生存期组件
	f2dWindowClass m_WinClass;
	f2dFileSysImpl m_FileSys;

	// 组件
	f2dWindowImpl*   m_pWindow;
	f2dSoundSysImpl* m_pSoundSys;
	f2dInputSysImpl* m_pInputSys;
	f2dRendererImpl* m_pRenderer;
	f2dVideoSysImpl* m_pVideoSys;
public: // 内部公开方法
	// 当异常被触发时调用该函数
	void ThrowException(const fcyException& e);
	void ThrowException(fuInt TimeTick, fcStr Src, fcStr Desc);
public: // 接口实现
	f2dEngineEventListener* GetListener();
	fResult SetListener(f2dEngineEventListener* pListener);

	fResult InitWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border);
	fResult InitSoundSys();
	fResult InitInputSys();
	fResult InitRenderer(fuInt BufferWidth, fuInt BufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel);
	fResult InitVideoSys();

	f2dWindow* GetMainWindow();
	f2dFileSys* GetFileSys();
	f2dSoundSys* GetSoundSys();
	f2dInputSys* GetInputSys();
	f2dRenderer* GetRenderer();
	f2dVideoSys* GetVideoSys();

	void Abort();
	void Run();

	fFloat GetFPS();
	fuInt GetMaxFPS();
	void SetMaxFPS(fuInt FPS);
	void DisablePresent();
public:
	f2dEngineImpl();
	~f2dEngineImpl();
};
