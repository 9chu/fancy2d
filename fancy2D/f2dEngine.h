////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEngine.h
/// @brief fancy2D引擎接口定义
/// @note  定义了f2D的引擎接口
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

// 所有组件
#include "f2dFileSys.h"
#include "f2dWindow.h"
#include "f2dInputSys.h"
#include "f2dSoundSys.h"
#include "f2dRenderer.h"
#include "f2dVideoSys.h"

/// @addtogroup f2d引擎
/// @brief fancy2d引擎接口
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D CPU信息
////////////////////////////////////////////////////////////////////////////////
struct f2dCPUInfo
{
	fcStr CPUString;
	fcStr CPUBrandString;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D引擎事件监听器
/// @note  通过覆写这个类来实现消息回调
////////////////////////////////////////////////////////////////////////////////
struct f2dEngineEventListener
{
	/// @brief 应用程序收到退出消息
	virtual void OnAppExit() {}
	/// @brief 引擎循环启动
	virtual void OnStartLoop() {}
	/// @brief 引擎循环终止
	virtual void OnAbortLoop() {}
	/// @brief     更新一帧
	/// @note      应当在这里完成更新操作
	/// @param[in] ElapsedTime 距离上一帧的流逝时间
	virtual void OnUpdate(fDouble ElapsedTime) {}
	/// @brief     渲染一帧
	/// @note      仅能在这里完成渲染操作
	/// @param[in] ElapsedTime 距离上一帧的流逝时间
	virtual void OnRender(fDouble ElapsedTime) {}
	/// @brief     FPS刷新
	/// @note      该函数不能保证严格的一秒一次调用，仅在计算出一秒的大致绘制频率后调用
	/// @param[in] FPS 新的FPS值
	virtual void OnFPSUpdate(fFloat FPS) {}
	/// @brief     内部异常被触发
	/// @note      当组件产生异常时会调用该函数
	/// @param[in] TimeTick 时间戳
	/// @param[in] Src      异常源
	/// @param[in] Desc     描述
	virtual void OnException(fuInt TimeTick, fcStr Src, fcStr Desc) {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D引擎
////////////////////////////////////////////////////////////////////////////////
struct f2dEngine : f2dInterface
{
	// --- 监听器 ---
	/// @brief 返回监听器
	virtual f2dEngineEventListener* GetListener()=0;

	/// @brief 设置监听器
	virtual fResult SetListener(f2dEngineEventListener* pListener)=0;

	// --- 组件初始化 ---
	/// @brief     初始化渲染窗口
	/// @note      渲染窗口必须在所有组件之前初始化
	/// @param[in] Pos      客户区位置
	/// @param[in] Title    窗口标题
	/// @param[in] Visiable 可见
	/// @param[in] Border   边框
	virtual fResult InitWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border)=0;
	
	/// @brief 初始化声音系统
	/// @note  需要预先初始化渲染窗口
	virtual fResult InitSoundSys()=0;

	/// @brief 初始化输入系统
	/// @note  需要预先初始化渲染窗口
	virtual fResult InitInputSys()=0;

	/// @brief 初始化渲染系统
	/// @note  需要预先初始化渲染窗口
	/// @param[in] BufferWidth  缓冲区宽度
	/// @param[in] BufferHeight 缓冲区高度
	/// @param[in] Windowed     窗口化
	/// @param[in] VSync        垂直同步
	/// @param[in] AALevel      抗锯齿等级
	virtual fResult InitRenderer(fuInt BufferWidth, fuInt BufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)=0;

	/// @brief 初始化视频系统
	/// @note  需要渲染系统支持
	virtual fResult InitVideoSys()=0;

	// --- 获得组件 ---
	virtual f2dWindow* GetMainWindow()=0;  ///< @brief 获得主窗口
	virtual f2dFileSys* GetFileSys()=0;    ///< @brief 获得文件系统
	virtual f2dSoundSys* GetSoundSys()=0;  ///< @brief 获得声音系统
	virtual f2dInputSys* GetInputSys()=0;  ///< @brief 获得输入系统
	virtual f2dRenderer* GetRenderer()=0;  ///< @brief 获得渲染系统
	virtual f2dVideoSys* GetVideoSys()=0;  ///< @brief 获得视频系统

	// --- 引擎控制 ---
	virtual void Abort()=0; ///< @brief 终止程序循环
	virtual void Run()=0;   ///< @brief 执行程序循环

	// --- 属性 ---
	virtual void GetCPUInfo(f2dCPUInfo& tInfo)=0; ///< @brief 返回CPU信息
	virtual fFloat GetFPS()=0;                    ///< @brief 获得FPS
	virtual fuInt GetMaxFPS()=0;                  ///< @brief 获得最大FPS
	virtual void SetMaxFPS(fuInt FPS)=0;          ///< @brief 设置最大FPS
	                                              ///< @note  设为0表示不限速
	virtual void DisablePresent()=0;              ///< @brief 关闭Present
};
/// @}
