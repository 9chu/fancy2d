////////////////////////////////////////////////////////////////////////////////
/// @file  f2dInputSysImpl.h
/// @brief fancy2D输入系统实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInputSysAPI.h"

#include "../f2dInputSys.h"

#include <vector>

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief 输入系统实现
////////////////////////////////////////////////////////////////////////////////
class f2dInputSysImpl :
	public fcyRefObjImpl<f2dInputSys>
{
private:
	f2dEngineImpl* m_pEngine;

	f2dInputSysAPI m_API;

	IDirectInput8* m_pDInput;
	HWND m_hWinHandle;
	std::vector<DIDEVICEINSTANCEW> m_MouseList;    // 鼠标列表
	std::vector<DIDEVICEINSTANCEW> m_KeyboardList; // 键盘列表
	std::vector<DIDEVICEINSTANCEW> m_GameCtrlList; // 控制器列表
private:
	static BOOL PASCAL enumMouse(LPCDIDEVICEINSTANCE pInfo, void* pThis);    // 枚举鼠标
	static BOOL PASCAL enumKeyboard(LPCDIDEVICEINSTANCE pInfo, void* pThis); // 枚举键盘
	static BOOL PASCAL enumGameCtrl(LPCDIDEVICEINSTANCE pInfo, void* pThis); // 枚举游戏手柄
private:
	void enumDevice();  // 枚举所有设备
public:
	fuInt GetDeviceCount(F2DINPUTDEVTYPE Type);
	fcStrW GetDeviceName(F2DINPUTDEVTYPE Type, fuInt Index);
	fcStrW GetDeviceProductName(F2DINPUTDEVTYPE Type, fuInt Index);
	
	fResult CreateMouse(fInt DevIndex, fBool bGlobalFocus, f2dInputMouse** pOut);
	fResult CreateKeyboard(fInt DevIndex, fBool bGlobalFocus, f2dInputKeyboard** pOut);
	fResult CreateJoystick(fInt DevIndex, fBool bGlobalFocus, f2dInputJoystick** pOut);
public:
	f2dInputSysImpl(f2dEngineImpl* pEngine);
protected:
	~f2dInputSysImpl();
};
