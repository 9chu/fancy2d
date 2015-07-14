////////////////////////////////////////////////////////////////////////////////
/// @file  f2dInputSys.h
/// @brief fancy2D����ϵͳ�ӿڶ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d����ϵͳ
/// @brief fancy2d��������ϵͳ
/// @{

struct f2dInputMouse;
struct f2dInputKeyboard;
struct f2dInputJoystick;

////////////////////////////////////////////////////////////////////////////////
/// @brief �����豸����
////////////////////////////////////////////////////////////////////////////////
enum F2DINPUTDEVTYPE
{
	F2DINPUTDEVTYPE_MOUSE,    ///< @brief �������
	F2DINPUTDEVTYPE_KEYBOARD, ///< @brief ��������
	F2DINPUTDEVTYPE_GAMECTRL  ///< @brief ��Ϸ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �����豸�ӿ�
////////////////////////////////////////////////////////////////////////////////
struct f2dInputDevice : public f2dInterface
{
	virtual f2dInputMouse* ToMouse()=0;       ///< @brief ת��������豸�����������ü�����
	virtual f2dInputKeyboard* ToKeyboard()=0; ///< @brief ת���������豸�����������ü�����
	virtual f2dInputJoystick* ToJoystick()=0; ///< @brief ת�����ֱ��豸�����������ü�����

	/// @brief �����豸״̬
	/// @note  �÷������������Զ����á�
	virtual fResult UpdateState()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����¼�������
////////////////////////////////////////////////////////////////////////////////
struct f2dInputMouseEventListener
{
	/// @brief     ���Xλ���ƶ�
	/// @param[in] Value �ƶ������ֵ
	virtual void OnMouseMoveX(fInt Value) {}

	/// @brief     ���Yλ���ƶ�
	/// @param[in] Value �ƶ������ֵ
	virtual void OnMouseMoveY(fInt Value) {}

	/// @brief     ���Zλ���ƶ�
	/// @param[in] Value �ƶ������ֵ
	virtual void OnMouseMoveZ(fInt Value) {}

	/// @brief ����������
	virtual void OnMouseLBtnDown() {}

	/// @brief ����Ҽ�����
	virtual void OnMouseRBtnDown() {}

	/// @brief ����м�����
	virtual void OnMouseMBtnDown() {}

	/// @brief     �����ⰴ������
	/// @param[in] Index ��������0��ʼ
	virtual void OnMouseAdditionBtnDown(fuInt Index) {}

	/// @brief �������ſ�
	virtual void OnMouseLBtnUp() {}

	/// @brief ����Ҽ��ſ�
	virtual void OnMouseRBtnUp() {}

	/// @brief ����м��ſ�
	virtual void OnMouseMBtnUp() {}

	/// @brief     �����ⰴ���ſ�
	/// @param[in] Index ��������0��ʼ
	virtual void OnMouseAdditionBtnUp(fuInt Index) {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����豸
////////////////////////////////////////////////////////////////////////////////
struct f2dInputMouse : public f2dInputDevice
{
	// --- ������ ---
	/// @brief ��ü�����
	virtual f2dInputMouseEventListener* GetListener()=0;

	/// @brief ���ü�����
	virtual fResult SetListener(f2dInputMouseEventListener* pListener)=0;
	
	// --- ״̬��ѯ ---
	virtual fBool IsLeftBtnDown()=0;                ///< @brief ����Ƿ���
	virtual fBool IsRightBtnDown()=0;               ///< @brief �Ҽ��Ƿ���
	virtual fBool IsMiddleBtnDown()=0;              ///< @brief �����Ƿ���
	virtual fBool IsAdditionBtnDown(fuInt Index)=0; ///< @brief     ���ⰴť�Ƿ���
	                                                ///< @param[in] Index ������0��ʼ
	virtual fInt GetOffsetX()=0;  ///< @brief �����ϴβ�ѯ�ۻ���X��λ��
	virtual fInt GetOffsetY()=0;  ///< @brief �����ϴβ�ѯ�ۻ���Y��λ��
	virtual fInt GetOffsetZ()=0;  ///< @brief �����ϴβ�ѯ�ۻ���Z��λ��
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ���̰�������
////////////////////////////////////////////////////////////////////////////////
enum F2DINPUTKEYCODE
{
	F2DINPUTKEYCODE_UNKNOWN=0x00,      ///< @brief δ֪
	F2DINPUTKEYCODE_ESCAPE=0x01,       ///< @brief ESC��
	F2DINPUTKEYCODE_1=0x02,            ///< @brief ������1
	F2DINPUTKEYCODE_2=0x03,            ///< @brief ������2
	F2DINPUTKEYCODE_3=0x04,            ///< @brief ������3
	F2DINPUTKEYCODE_4=0x05,            ///< @brief ������4
	F2DINPUTKEYCODE_5=0x06,            ///< @brief ������5
	F2DINPUTKEYCODE_6=0x07,            ///< @brief ������6
	F2DINPUTKEYCODE_7=0x08,            ///< @brief ������7
	F2DINPUTKEYCODE_8=0x09,            ///< @brief ������8
	F2DINPUTKEYCODE_9=0x0A,            ///< @brief ������9
	F2DINPUTKEYCODE_0=0x0B,            ///< @brief ������0
	F2DINPUTKEYCODE_MINUS=0x0C,        ///< @brief ������-
	F2DINPUTKEYCODE_EQUALS=0x0D,       ///< @brief =
	F2DINPUTKEYCODE_BACK=0x0E,         ///< @brief �˸�
	F2DINPUTKEYCODE_TAB=0x0F,          ///< @brief TAB
	F2DINPUTKEYCODE_Q=0x10,            ///< @brief Q
	F2DINPUTKEYCODE_W=0x11,            ///< @brief W
	F2DINPUTKEYCODE_E=0x12,            ///< @brief E
	F2DINPUTKEYCODE_R=0x13,            ///< @brief R
	F2DINPUTKEYCODE_T=0x14,            ///< @brief T
	F2DINPUTKEYCODE_Y=0x15,            ///< @brief Y
	F2DINPUTKEYCODE_U=0x16,            ///< @brief U
	F2DINPUTKEYCODE_I=0x17,            ///< @brief I
	F2DINPUTKEYCODE_O=0x18,            ///< @brief O
	F2DINPUTKEYCODE_P=0x19,            ///< @brief P
	F2DINPUTKEYCODE_LBRACKET=0x1A,     ///< @brief [
	F2DINPUTKEYCODE_RBRACKET=0x1B,     ///< @brief ]
	F2DINPUTKEYCODE_RETURN=0x1C,       ///< @brief �س�
	F2DINPUTKEYCODE_LCONTROL=0x1D,     ///< @brief ��Ctrl
	F2DINPUTKEYCODE_A=0x1E,            ///< @brief A
	F2DINPUTKEYCODE_S=0x1F,            ///< @brief S
	F2DINPUTKEYCODE_D=0x20,            ///< @brief D
	F2DINPUTKEYCODE_F=0x21,            ///< @brief F
	F2DINPUTKEYCODE_G=0x22,            ///< @brief G
	F2DINPUTKEYCODE_H=0x23,            ///< @brief H
	F2DINPUTKEYCODE_J=0x24,            ///< @brief J
	F2DINPUTKEYCODE_K=0x25,            ///< @brief K
	F2DINPUTKEYCODE_L=0x26,            ///< @brief L
	F2DINPUTKEYCODE_SEMICOLON=0x27,    ///< @brief ;
	F2DINPUTKEYCODE_APOSTROPHE=0x28,   ///< @brief '
	F2DINPUTKEYCODE_GRAVE=0x29,        ///< @brief `
	F2DINPUTKEYCODE_LSHIFT=0x2A,       ///< @brief ��shift
	F2DINPUTKEYCODE_BACKSLASH=0x2B,    ///< @brief ��б��
	F2DINPUTKEYCODE_Z=0x2C,            ///< @brief Z
	F2DINPUTKEYCODE_X=0x2D,            ///< @brief X
	F2DINPUTKEYCODE_C=0x2E,            ///< @brief C
	F2DINPUTKEYCODE_V=0x2F,            ///< @brief V
	F2DINPUTKEYCODE_B=0x30,            ///< @brief B
	F2DINPUTKEYCODE_N=0x31,            ///< @brief N
	F2DINPUTKEYCODE_M=0x32,            ///< @brief M
	F2DINPUTKEYCODE_COMMA=0x33,        ///< @brief ����
	F2DINPUTKEYCODE_PERIOD=0x34,       ///< @brief ���
	F2DINPUTKEYCODE_SLASH=0x35,        ///< @brief /
	F2DINPUTKEYCODE_RSHIFT=0x36,       ///< @brief ��shift
	F2DINPUTKEYCODE_MULTIPLY=0x37,     ///< @brief �˺�
	F2DINPUTKEYCODE_LMENU=0x38,        ///< @brief ��alt
	F2DINPUTKEYCODE_SPACE=0x39,        ///< @brief �ո�
	F2DINPUTKEYCODE_CAPITAL=0x3A,      ///< @brief ��д
	F2DINPUTKEYCODE_F1=0x3B,           ///< @brief F1
	F2DINPUTKEYCODE_F2=0x3C,           ///< @brief F2
	F2DINPUTKEYCODE_F3=0x3D,           ///< @brief F3
	F2DINPUTKEYCODE_F4=0x3E,           ///< @brief F4
	F2DINPUTKEYCODE_F5=0x3F,           ///< @brief F5
	F2DINPUTKEYCODE_F6=0x40,           ///< @brief F6
	F2DINPUTKEYCODE_F7=0x41,           ///< @brief F7
	F2DINPUTKEYCODE_F8=0x42,           ///< @brief F8
	F2DINPUTKEYCODE_F9=0x43,           ///< @brief F9
	F2DINPUTKEYCODE_F10=0x44,          ///< @brief F10
	F2DINPUTKEYCODE_NUMLOCK=0x45,      ///< @brief ������
	F2DINPUTKEYCODE_SCROLL=0x46,       ///< @brief ������
	F2DINPUTKEYCODE_NUMPAD7=0x47,      ///< @brief ���ּ���7
	F2DINPUTKEYCODE_NUMPAD8=0x48,      ///< @brief ���ּ���8
	F2DINPUTKEYCODE_NUMPAD9=0x49,      ///< @brief ���ּ���9
	F2DINPUTKEYCODE_SUBTRACT=0x4A,     ///< @brief ���ּ���-
	F2DINPUTKEYCODE_NUMPAD4=0x4B,      ///< @brief ���ּ���4
	F2DINPUTKEYCODE_NUMPAD5=0x4C,      ///< @brief ���ּ���5
	F2DINPUTKEYCODE_NUMPAD6=0x4D,      ///< @brief ���ּ���6
	F2DINPUTKEYCODE_ADD=0x4E,          ///< @brief ���ּ���+
	F2DINPUTKEYCODE_NUMPAD1=0x4F,      ///< @brief ���ּ���1
	F2DINPUTKEYCODE_NUMPAD2=0x50,      ///< @brief ���ּ���2
	F2DINPUTKEYCODE_NUMPAD3=0x51,      ///< @brief ���ּ���3
	F2DINPUTKEYCODE_NUMPAD0=0x52,      ///< @brief ���ּ���0
	F2DINPUTKEYCODE_DECIMAL=0x53,      ///< @brief ���ּ���.
	F2DINPUTKEYCODE_OEM_102=0x56,      
	F2DINPUTKEYCODE_F11=0x57,          ///< @brief F11
	F2DINPUTKEYCODE_F12=0x58,          ///< @brief F12
	F2DINPUTKEYCODE_F13=0x64,          ///< @brief F13
	F2DINPUTKEYCODE_F14=0x65,          ///< @brief F14
	F2DINPUTKEYCODE_F15=0x66,          ///< @brief F15
	F2DINPUTKEYCODE_KANA=0x70,
	F2DINPUTKEYCODE_ABNT_C1=0x73,
	F2DINPUTKEYCODE_CONVERT=0x79,
	F2DINPUTKEYCODE_NOCONVERT=0x7B,
	F2DINPUTKEYCODE_YEN=0x7D,
	F2DINPUTKEYCODE_ABNT_C2=0x7E,
	F2DINPUTKEYCODE_NUMPADEQUALS=0x8D,
	F2DINPUTKEYCODE_PREVTRACK=0x90,
	F2DINPUTKEYCODE_AT=0x91,
	F2DINPUTKEYCODE_COLON=0x92,
	F2DINPUTKEYCODE_UNDERLINE=0x93,
	F2DINPUTKEYCODE_KANJI=0x94,
	F2DINPUTKEYCODE_STOP=0x95,
	F2DINPUTKEYCODE_AX=0x96,
	F2DINPUTKEYCODE_UNLABELED=0x97,
	F2DINPUTKEYCODE_NEXTTRACK=0x99,
	F2DINPUTKEYCODE_NUMPADENTER=0x9C,
	F2DINPUTKEYCODE_RCONTROL=0x9D,
	F2DINPUTKEYCODE_MUTE=0xA0,
	F2DINPUTKEYCODE_CALCULATOR=0xA1,
	F2DINPUTKEYCODE_PLAYPAUSE=0xA2,
	F2DINPUTKEYCODE_MEDIASTOP=0xA4,
	F2DINPUTKEYCODE_VOLUMEDOWN=0xAE,
	F2DINPUTKEYCODE_VOLUMEUP=0xB0,
	F2DINPUTKEYCODE_WEBHOME=0xB2,
	F2DINPUTKEYCODE_NUMPADCOMMA=0xB3,
	F2DINPUTKEYCODE_DIVIDE=0xB5,
	F2DINPUTKEYCODE_SYSRQ=0xB7,
	F2DINPUTKEYCODE_RMENU=0xB8,
	F2DINPUTKEYCODE_PAUSE=0xC5,
	F2DINPUTKEYCODE_HOME=0xC7,
	F2DINPUTKEYCODE_UP=0xC8,
	F2DINPUTKEYCODE_PRIOR=0xC9,
	F2DINPUTKEYCODE_LEFT=0xCB,
	F2DINPUTKEYCODE_RIGHT=0xCD,
	F2DINPUTKEYCODE_END=0xCF,
	F2DINPUTKEYCODE_DOWN=0xD0,
	F2DINPUTKEYCODE_NEXT=0xD1,
	F2DINPUTKEYCODE_INSERT=0xD2,
	F2DINPUTKEYCODE_DELETE=0xD3,
	F2DINPUTKEYCODE_LWIN=0xDB,
	F2DINPUTKEYCODE_RWIN=0xDC,
	F2DINPUTKEYCODE_APPS=0xDD,
	F2DINPUTKEYCODE_POWER=0xDE,
	F2DINPUTKEYCODE_SLEEP=0xDF,
	F2DINPUTKEYCODE_WAKE=0xE3,
	F2DINPUTKEYCODE_WEBSEARCH=0xE5,
	F2DINPUTKEYCODE_WEBFAVORITES=0xE6,
	F2DINPUTKEYCODE_WEBREFRESH=0xE7,
	F2DINPUTKEYCODE_WEBSTOP=0xE8,
	F2DINPUTKEYCODE_WEBFORWARD=0xE9,
	F2DINPUTKEYCODE_WEBBACK=0xEA,
	F2DINPUTKEYCODE_MYCOMPUTER=0xEB,
	F2DINPUTKEYCODE_MAIL=0xEC,
	F2DINPUTKEYCODE_MEDIASELECT=0xED
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �����¼�������
////////////////////////////////////////////////////////////////////////////////
struct f2dInputKeyboardEventListener
{
	/// @brief     ���̰�������
	/// @param[in] KeyCode ��������
	virtual void OnKeyboardBtnDown(F2DINPUTKEYCODE KeyCode) {}

	/// @brief     ���̰����ſ�
	/// @param[in] KeyCode ��������
	virtual void OnKeyboardBtnUp(F2DINPUTKEYCODE KeyCode) {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �����豸
////////////////////////////////////////////////////////////////////////////////
struct f2dInputKeyboard : public f2dInputDevice
{
	// --- ������ ---
	/// @brief ���ؼ�����
	virtual f2dInputKeyboardEventListener* GetListener()=0;
	/// @brief ���ü�����
	virtual fResult SetListener(f2dInputKeyboardEventListener* pListener)=0;
	
	// --- ״̬��ѯ ---
	virtual fBool IsKeyDown(F2DINPUTKEYCODE KeyCode)=0; ///< @brief     �жϰ����Ƿ���
	                                                    ///< @param[in] KeyCode ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �ֱ��¼�������
////////////////////////////////////////////////////////////////////////////////
struct f2dInputJoystickEventListener
{
	/// @brief     ��ť����
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Index ��������
	virtual void OnJoystickBtnDown(f2dInputJoystick* p, fuInt Index) {}

	/// @brief     ��ť�ſ�
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Index ��������
	virtual void OnJoystickBtnUp(f2dInputJoystick* p, fuInt Index) {}

	/// @brief     X��λ�ñ仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickXAxisPosChange(f2dInputJoystick* p, fFloat Value) {}

	/// @brief     Y��λ�ñ仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickYAxisPosChange(f2dInputJoystick* p, fFloat Value) {}

	/// @brief     Z��λ�ñ仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickZAxisPosChange(f2dInputJoystick* p, fFloat Value) {}

	/// @brief     X����ת�仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickXAxisRotationChange(f2dInputJoystick* p, fFloat Value) {}

	/// @brief     Y����ת�仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickYAxisRotationChange(f2dInputJoystick* p, fFloat Value) {}

	/// @brief     Z����ת�仯
	/// @param[in] p     �ֱ�ָ��
	/// @param[in] Value �µ�λ��
	virtual void OnJoystickZAxisRotationChange(f2dInputJoystick* p, fFloat Value) {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief �ֱ��豸
////////////////////////////////////////////////////////////////////////////////
struct f2dInputJoystick : public f2dInputDevice
{
	// --- ������ ---
	/// @brief ���ؼ�����
	virtual f2dInputJoystickEventListener* GetListener()=0;
	/// @brief ���ü�����
	virtual fResult SetListener(f2dInputJoystickEventListener* pListener)=0;

	// --- ״̬��ѯ ---
	virtual fFloat GetXPosition()=0;  ///< @brief �ֱ�Xλ�ƣ�����[-1,1]
	virtual fFloat GetYPosition()=0;  ///< @brief �ֱ�Yλ�ƣ�����[-1,1]
	virtual fFloat GetZPosition()=0;  ///< @brief �ֱ�Zλ�ƣ�����[-1,1]
	virtual fFloat GetXRotation()=0;  ///< @brief �ֱ�X��ת������[-1,1]
	virtual fFloat GetYRotation()=0;  ///< @brief �ֱ�Y��ת������[-1,1]
	virtual fFloat GetZRotation()=0;  ///< @brief �ֱ�Z��ת������[-1,1]
	virtual fBool IsButtonDown(fuInt Index)=0;  ///< @brief ��鰴ť�Ƿ���
	virtual fInt GetSlider(fuInt Index)=0;      ///< @brief ��û���ֵ
	virtual fuInt GetPOV(fuInt Index)=0;        ///< @brief ���POVֵ

	virtual fFloat GetDeadZone()=0;                  ///< @brief ����������[0,1]֮��ֵ
	virtual fResult SetDeadZone(fFloat Percent)=0;   ///< @brief ����������[0,1]֮��ֵ
	                                                 ///< @note  �����ֱ���С��Ӧ������
	virtual fFloat GetSaturation()=0;                ///< @brief ���ر���ֵ��[0,1]֮��ֵ
	virtual fResult SetSaturation(fFloat Percent)=0; ///< @brief ���ñ���ֵ��[0,1]֮��ֵ
	                                                 ///< @note  �����ֱ������Ӧ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D����ϵͳ
////////////////////////////////////////////////////////////////////////////////
struct f2dInputSys
{
	/// @brief     �����豸����
	/// @param[in] Type �豸����
	virtual fuInt GetDeviceCount(F2DINPUTDEVTYPE Type)=0;

	/// @brief     �����豸����
	/// @param[in] Type  �豸����
	/// @param[in] Index �豸����
	virtual fcStrW GetDeviceName(F2DINPUTDEVTYPE Type, fuInt Index)=0;

	/// @brief     �����豸��Ʒ����
	/// @param[in] Type  �豸����
	/// @param[in] Index �豸����
	virtual fcStrW GetDeviceProductName(F2DINPUTDEVTYPE Type, fuInt Index)=0;
	
	/// @brief      �������
	/// @param[in]  DevIndex     ����Ĭ������豸ʱ��Ϊ-1
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrueʱ�����ڷǼ���״̬ʱ��ɲ�׽��Ϣ
	/// @param[out] pOut         �������豸ָ��
	virtual fResult CreateMouse(fInt DevIndex, fBool bGlobalFocus, f2dInputMouse** pOut)=0;

	/// @brief      ��������
	/// @param[in]  DevIndex     ����Ĭ������豸ʱ��Ϊ-1
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrueʱ�����ڷǼ���״̬ʱ��ɲ�׽��Ϣ
	/// @param[out] pOut         �������豸ָ��
	virtual fResult CreateKeyboard(fInt DevIndex, fBool bGlobalFocus, f2dInputKeyboard** pOut)=0;

	/// @brief      ��������
	/// @param[in]  DevIndex     ��0��ʼ��������GetDeviceCountö��
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrueʱ�����ڷǼ���״̬ʱ��ɲ�׽��Ϣ
	/// @param[out] pOut         �������豸ָ��
	virtual fResult CreateJoystick(fInt DevIndex, fBool bGlobalFocus, f2dInputJoystick** pOut)=0;
};

/// @}
