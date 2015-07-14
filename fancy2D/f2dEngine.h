////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEngine.h
/// @brief fancy2D����ӿڶ���
/// @note  ������f2D������ӿ�
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

// �������
#include "f2dFileSys.h"
#include "f2dWindow.h"
#include "f2dInputSys.h"
#include "f2dSoundSys.h"
#include "f2dRenderer.h"
#include "f2dVideoSys.h"

/// @addtogroup f2d����
/// @brief fancy2d����ӿ�
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D CPU��Ϣ
////////////////////////////////////////////////////////////////////////////////
struct f2dCPUInfo
{
	fcStr CPUString;      ///< @brief ��ʶ
	fcStr CPUBrandString; ///< @brief ��Ʒ��
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D FPS������
/// @note  ���̰߳�ȫ
////////////////////////////////////////////////////////////////////////////////
struct f2dFPSController
{
	/// @brief �������FPS
	/// @note  0��ʾ������
	virtual fuInt GetLimitedFPS()=0;
	
	/// @brief �������FPS
	/// @param iMaxFPS ���FPS
	virtual void SetLimitedFPS(fuInt iMaxFPS)=0;
	
	/// @brief ����FPS
	virtual fDouble GetFPS()=0;

	/// @brief ������֡��
	virtual fuInt GetTotalFrame()=0;
	
	/// @brief ����������ʱ��
	/// @note  ����Ϊ��λ
	virtual fDouble GetTotalTime()=0;
	
	/// @brief ����ƽ��FPS
	virtual fDouble GetAvgFPS()=0;
	
	/// @brief �������FPS
	virtual fDouble GetMaxFPS()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ��Ϣ����
////////////////////////////////////////////////////////////////////////////////
enum F2DMSGTYPE
{
	F2DMSG_USER = 0,                      ///< @brief �û���Ϣ
	
	F2DMSG_APP_ONEXIT      = 1,           ///< @brief �����˳���Ϣ
	F2DMSG_APP_ONEXCEPTION = 2,           ///< @brief �����쳣��Ϣ (fuInt Time,fcStr Src,fcStr Desc)

	F2DMSG_WINDOW_ONCLOSE        = 10,    ///< @brief ���ڹر���Ϣ
	F2DMSG_WINDOW_ONPAINT        = 11,    ///< @brief �����ػ���Ϣ
	F2DMSG_WINDOW_ONGETFOCUS     = 12,    ///< @brief ��ý�����Ϣ
	F2DMSG_WINDOW_ONLOSTFOCUS    = 13,    ///< @brief ��ʧ������Ϣ
	F2DMSG_WINDOW_ONRESIZE       = 14,    ///< @brief ����������Ϣ (fuInt Width, fuInt Height)
	F2DMSG_WINDOW_ONCHARINPUT    = 15,    ///< @brief �ַ�������Ϣ (fCharW Char)
	F2DMSG_WINDOW_ONMOUSEMOVE    = 16,    ///< @brief ����ƶ�     (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSELUP     = 17,    ///< @brief �������ſ� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSELDOWN   = 18,    ///< @brief ���������� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSELDOUBLE = 19,    ///< @brief ������˫�� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSERUP     = 20,    ///< @brief ����Ҽ��ſ� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSERDOWN   = 21,    ///< @brief ����Ҽ����� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSERDOUBLE = 22,    ///< @brief ����Ҽ�˫�� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSEMUP     = 23,    ///< @brief ����м��ſ� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSEMDOWN   = 24,    ///< @brief ����м����� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSEMDOUBLE = 25,    ///< @brief ����м�˫�� (fInt X, fInt Y)
	F2DMSG_WINDOW_ONMOUSEWHEEL   = 26,    ///< @brief ������     (fInt X, fInt Y, fDouble Wheel)
	F2DMSG_WINDOW_ONKEYUP        = 27,    ///< @brief �����ſ�     (fuInt KeyCode)
	F2DMSG_WINDOW_ONKEYDOWN      = 28,    ///< @brief ��������     (fuInt KeyCode)

	F2DMSG_IME_ONCLOSE            = 30,   ///< @brief IME�ر�
	F2DMSG_IME_ONACTIVATE         = 31,   ///< @brief IME����       (fcStrW Desc)
	F2DMSG_IME_ONSTARTCOMPOSITION = 32,   ///< @brief IME��ʼƴд
	F2DMSG_IME_ONENDCOMPOSITION   = 33,   ///< @brief IME����ƴд
	F2DMSG_IME_ONCOMPOSITION      = 34,   ///< @brief IME����ƴд   (fcStrW CompStr, fCharW Char)
	F2DMSG_IME_ONOPENCANDIDATE    = 35,   ///< @brief IME�򿪺�ѡ�� (f2dIMECandidateList* Ptr)
	F2DMSG_IME_ONCLOSECANDIDATE   = 36,   ///< @brief IME�رպ�ѡ�� (f2dIMECandidateList* Ptr)
	F2DMSG_IME_ONCHANGECANDIDATE  = 37,   ///< @brief IME�ı��ѡ�� (f2dIMECandidateList* Ptr)

	F2DMSG_RENDER_ONDEVLOST   = 50,       ///< @brief �豸��ʧ��Ϣ
	F2DMSG_RENDER_ONDEVRESET  = 51,       ///< @brief �豸������Ϣ 
	
	F2DMSG_MOUSE_ONOFFSET     = 60,       ///< @brief ���λ����Ϣ (fInt X, fInt Y, fInt Z)
	F2DMSG_MOUSE_ONLBTNUP     = 61,       ///< @brief �������ſ�
	F2DMSG_MOUSE_ONLBTNDOWN   = 62,       ///< @brief ����������
	F2DMSG_MOUSE_ONRBTNUP     = 63,       ///< @brief ����Ҽ��ſ�
	F2DMSG_MOUSE_ONRBTNDOWN   = 64,       ///< @brief ����Ҽ�����
	F2DMSG_MOUSE_ONMBTNUP     = 65,       ///< @brief ����м��ſ�
	F2DMSG_MOUSE_ONMBTNDOWN   = 66,       ///< @brief ����м�����
	F2DMSG_MOUSE_ONADDBTNUP   = 67,       ///< @brief �����ⰴ���ſ� (fuInt Index)
	F2DMSG_MOUSE_ONADDBTNDOWN = 68,       ///< @brief �����ⰴ������ (fuInt Index)
	
	F2DMSG_KEYBOARD_ONKEYUP   = 70,       ///< @brief �����ſ� (F2DINPUTKEYCODE Key)
	F2DMSG_KEYBOARD_ONKEYDOWN = 71,       ///< @brief �������� (F2DINPUTKEYCODE Key)

	F2DMSG_JOYSTICK_ONBUTTONUP   = 80,    ///< @brief �ֱ������ſ� (fuInt Index, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONBUTTONDOWN = 81,    ///< @brief �ֱ��������� (fuInt Index, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONXPOSCHANGE = 82,    ///< @brief X��λ�øı�  (fDouble Value, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONYPOSCHANGE = 83,    ///< @brief Y��λ�øı�  (fDouble Value, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONZPOSCHANGE = 84,    ///< @brief Z��λ�øı�  (fDouble Value, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONXROTCHANGE = 85,    ///< @brief X����ת�ı�  (fDouble Value, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONYROTCHANGE = 86,    ///< @brief Y����ת�ı�  (fDouble Value, f2dInputJoystick*)
	F2DMSG_JOYSTICK_ONZROTCHANGE = 87     ///< @brief Z����ת�ı�  (fDouble Value, f2dInputJoystick*)
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ��Ϣ
////////////////////////////////////////////////////////////////////////////////
struct f2dMsg
{
	F2DMSGTYPE Type;  ///< @brief ��Ϣ����
	fuLong Param1;    ///< @brief ����1
	fuLong Param2;    ///< @brief ����2
	fuLong Param3;    ///< @brief ����3
	fuLong Param4;    ///< @brief ����4
};

////////////////////////////////////////////////////////////////////////////////
/// @brief   fancy2D ��Ϣ��
/// @note    ��Ϣ�û���Update���������������Ϣ�����ն����ڴ�
/// @warning ���̰߳�ȫ
////////////////////////////////////////////////////////////////////////////////
struct f2dMsgPump
{
	/// @brief ��Ϣ���Ƿ��
	virtual fBool IsEmpty()=0;
	/// @brief ����һ����Ϣ
	virtual fResult GetMsg(f2dMsg* MsgOut)=0;
	/// @brief ��ѯһ����Ϣ
	virtual fResult PeekMsg(f2dMsg* MsgOut)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ���߳�ί��
/// @note  ��������ί�и����߳�����
////////////////////////////////////////////////////////////////////////////////
struct f2dMainThreadDelegate : public f2dInterface
{
	/// @brief ִ�к���
	virtual void Excute()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D�����¼�������
/// @note  ͨ����д�������ʵ����Ϣ�ص�
////////////////////////////////////////////////////////////////////////////////
struct f2dEngineEventListener
{
	/// @brief     ����һ֡
	/// @note      Ӧ����������ɸ��²���
	/// @warning   �÷������ܿ��߳�
	/// @param[in] ElapsedTime ������һ֡������ʱ��
	/// @return    ����false�˳�ѭ��
	virtual fBool OnUpdate(fDouble ElapsedTime, f2dFPSController* pFPSController, f2dMsgPump* pMsgPump) { return true; }
	
	/// @brief     ��Ⱦһ֡
	/// @note      ���������������Ⱦ����
	/// @warning   �÷������ܿ��߳�
	/// @param[in] ElapsedTime ������һ֡������ʱ��
	/// @return    ����true���»���
	virtual fBool OnRender(fDouble ElapsedTime, f2dFPSController* pFPSController) { return true; }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D������߳�ģʽ
////////////////////////////////////////////////////////////////////////////////
enum F2DENGTHREADMODE
{
	F2DENGTHREADMODE_SINGLETHREAD,    ///< @brief ���߳�ģʽ
	F2DENGTHREADMODE_MULTITHREAD,     ///< @brief OnUpdate��OnRender��ͬһ�̣߳�����һ��FPS������
	F2DENGTHREADMODE_FULLMULTITHREAD  ///< @brief OnUpdate��OnRender�ֿ������̣߳�������FPS������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D����
////////////////////////////////////////////////////////////////////////////////
struct f2dEngine : f2dInterface
{
	// --- ������ ---
	/// @brief ���ؼ�����
	virtual f2dEngineEventListener* GetListener()=0;

	/// @brief ���ü�����
	virtual fResult SetListener(f2dEngineEventListener* pListener)=0;

	// --- �����ʼ�� ---
	/// @brief     ��ʼ����Ⱦ����
	/// @warning   ��ֹ���̳߳�ʼ��
	/// @note      ��Ⱦ���ڱ������������֮ǰ��ʼ��
	/// @param[in] Pos      �ͻ���λ��
	/// @param[in] Title    ���ڱ���
	/// @param[in] Visiable �ɼ�
	/// @param[in] Border   �߿�
	virtual fResult InitWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border)=0;
	
	/// @brief ��ʼ������ϵͳ
	/// @note  ��ҪԤ�ȳ�ʼ����Ⱦ����
	virtual fResult InitSoundSys()=0;

	/// @brief ��ʼ������ϵͳ
	/// @note  ��ҪԤ�ȳ�ʼ����Ⱦ����
	virtual fResult InitInputSys()=0;

	/// @brief     ��ʼ����Ⱦϵͳ
	/// @warning   ��ֹ���̳߳�ʼ��
	/// @note      ��ҪԤ�ȳ�ʼ����Ⱦ����
	/// @param[in] BufferWidth  ���������
	/// @param[in] BufferHeight �������߶�
	/// @param[in] Windowed     ���ڻ�
	/// @param[in] VSync        ��ֱͬ��
	/// @param[in] AALevel      ����ݵȼ�
	virtual fResult InitRenderer(fuInt BufferWidth, fuInt BufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)=0;

	/// @brief ��ʼ����Ƶϵͳ
	/// @note  ��Ҫ��Ⱦϵͳ֧��
	virtual fResult InitVideoSys()=0;

	// --- ������ ---
	virtual f2dWindow* GetMainWindow()=0;  ///< @brief ���������
	virtual f2dFileSys* GetFileSys()=0;    ///< @brief ����ļ�ϵͳ
	virtual f2dSoundSys* GetSoundSys()=0;  ///< @brief �������ϵͳ
	virtual f2dInputSys* GetInputSys()=0;  ///< @brief �������ϵͳ
	virtual f2dRenderer* GetRenderer()=0;  ///< @brief �����Ⱦϵͳ
	virtual f2dVideoSys* GetVideoSys()=0;  ///< @brief �����Ƶϵͳ

	// --- ���� ---
	virtual void ClearLstErr()=0;      ///< @brief �����һ�δ���
	virtual fuInt GetLastErrTime()=0;  ///< @brief ������һ�δ����ʱ��
	virtual fcStr GetLastErrSrc()=0;   ///< @brief ������һ�δ������Դ
	virtual fcStr GetLastErrDesc()=0;  ///< @brief ������һ�δ��������

	// --- ������� ---
	/// @brief ��ֹ����ѭ��
	/// @note  �̰߳�ȫ
	/// @note  ����Update���ؽ����Σ���һ֡��������ֹѭ��
	virtual void Abort()=0;

	/// @brief ִ�г���ѭ��
	/// @param[in] ThreadMode   �߳�ģʽ
	/// @param[in] UpdateMaxFPS �����߳����FPS
	/// @param[in] RenderMaxFPS ��Ⱦ�߳����FPS������ȫ���߳�ģʽ��Ч��
	virtual void Run(F2DENGTHREADMODE ThreadMode, fuInt UpdateMaxFPS=0, fuInt RenderMaxFPS=0)=0;

	/// @brief �����߳�ģʽ
	virtual F2DENGTHREADMODE GetCurrentThreadMode()=0;

	// --- ��Ϣ���� ---
	/// @brief     ������Ϣ
	/// @note      �̰߳�ȫ
	/// @param[in] Msg     ��Ϣ
	/// @param[in] pMemObj �ɿգ���Ϣ���ӵ��ڴ�����
	virtual fResult SendMsg(const f2dMsg& Msg, f2dInterface* pMemObj = NULL)=0;
	
	/// @brief     ������Ϣ
	/// @note      �̰߳�ȫ
	/// @param[in] Type    ��Ϣ����
	/// @param[in] Param1  ����1
	/// @param[in] Param2  ����2
	/// @param[in] Param3  ����3
	/// @param[in] Param4  ����4
	/// @param[in] pMemObj �ɿգ���Ϣ���ӵ��ڴ�����
	virtual fResult SendMsg(F2DMSGTYPE Type, fuLong Param1 = 0, fuLong Param2 = 0, fuLong Param3 = 0, fuLong Param4 = 0, f2dInterface* pMemObj = NULL)=0;

	/// @brief     ����ί�з���
	/// @param[in] pDelegate ί���¼�����
	virtual fResult InvokeDelegate(f2dMainThreadDelegate* pDelegate)=0;
	
	/// @brief     ����ί�з������ȴ�
	/// @param[in] pDelegate ί���¼�����
	virtual fResult InvokeDelegateAndWait(f2dMainThreadDelegate* pDelegate)=0;

	// --- ��չ���� ---
	/// @brief     ����CPU��Ϣ
	/// @param[in] Info �����CPU��Ϣ
	virtual void GetCPUInfo(f2dCPUInfo& Info)=0;
};
/// @}
