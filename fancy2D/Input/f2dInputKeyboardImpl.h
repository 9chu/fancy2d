////////////////////////////////////////////////////////////////////////////////
/// @file  f2dInputKeyboardImpl.h
/// @brief fancy2D输入系统 键盘实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyRefObj.h"
#include "fcyIO\fcyStream.h"

#include "../f2dInputSys.h"

#include "f2dInputSysAPI.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief 键盘设备实现
////////////////////////////////////////////////////////////////////////////////
class f2dInputKeyboardImpl :
	public fcyRefObjImpl<f2dInputKeyboard>
{
	friend class f2dInputSysImpl;
private:
	static const fuInt BufferSize; 
	static const DIDATAFORMAT DIDF_Keyboard;
	static const DIOBJECTDATAFORMAT DIODF_Keyboard[256];
private:
	IDirectInputDevice8* m_pDev;
	f2dInputKeyboardEventListener* m_pListener;
	fBool m_ButtonState[256];
public:
	f2dInputMouse* ToMouse();
	f2dInputKeyboard* ToKeyboard();
	f2dInputJoystick* ToJoystick();

	fResult UpdateState(); 

	f2dInputKeyboardEventListener* GetListener();
	fResult SetListener(f2dInputKeyboardEventListener* pListener);
	
	fBool IsKeyDown(F2DINPUTKEYCODE KeyCode);
protected: // 禁止直接new/delete
	f2dInputKeyboardImpl(IDirectInput8* pDev, HWND Win, const GUID& pGUID, fBool bGlobalFocus);
	~f2dInputKeyboardImpl();
};
