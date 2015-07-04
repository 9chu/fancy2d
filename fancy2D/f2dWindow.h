////////////////////////////////////////////////////////////////////////////////
/// @file  f2dWindow.h
/// @brief fancy2D��Ⱦ���ڽӿڶ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d��Ⱦ����
/// @brief fancy2d������Ⱦ����
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2d��Ⱦ���ڱ߿�����
////////////////////////////////////////////////////////////////////////////////
enum F2DWINBORDERTYPE
{
	F2DWINBORDERTYPE_NONE,
	F2DWINBORDERTYPE_FIXED,
	F2DWINBORDERTYPE_SIZEABLE
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2d��Ⱦ����IME��Ϣ
////////////////////////////////////////////////////////////////////////////////
enum F2DIMEINFO
{
	F2DIMEINFO_NULL,
	F2DIMEINFO_CANDIDATECOUNT,  ///< @brief ��ѡ�ʸ���
	F2DIMEINFO_CANDIDATEINDEX,  ///< @brief ��ǰѡ�еĺ�ѡ������
	F2DIMEINFO_PAGESIZE,        ///< @brief һҳ�ĺ�ѡ�ʸ���
	F2DIMEINFO_PAGESTART        ///< @brief ��ǰҳ�濪ʼ�ĺ�ѡ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2d��ѡ���б�
////////////////////////////////////////////////////////////////////////////////
struct f2dIMECandidateList
{
	virtual fuInt GetCount()=0;     ///< @brief ���غ�ѡ������
	virtual fuInt GetCurIndex()=0;  ///< @brief ���ص�ǰ��ѡ������
	virtual fuInt GetPageSize()=0;  ///< @brief ���ص���ҳ���ѡ������
	virtual fuInt GetPageStart()=0; ///< @brief ���ص�ǰҳ���׸���ѡ�ʵ�����
	
	/// @brief ��ѯ��ѡ��
	virtual fcStrW GetCandidateStr(fuInt Index)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��Ⱦ�����¼�������
/// @note  ͨ����д�������ʵ����Ϣ�ص�
////////////////////////////////////////////////////////////////////////////////
struct f2dWindowEventListener
{
	/// @brief ���ڹر��¼�
	virtual void OnClose() {}

	/// @brief �����ػ��¼�
	virtual void OnPaint() {}

	/// @brief     ���ڴ�С�ı��¼�
	/// @param[in] ClientWidth  �µĿͻ������
	/// @param[in] ClientHeight �µĿͻ����߶�
	virtual void OnSize(fuInt ClientWidth, fuInt ClientHeight) {}

	/// @brief     ��������
	/// @param[in] KeyCode �������룬�����MSDN
	/// @param[in] Flag    �������ԣ������MSDN
	virtual void OnKeyDown(fuInt KeyCode, fuInt Flag) {}

	/// @brief     �����ſ�
	/// @param[in] KeyCode �������룬�����MSDN
	/// @param[in] Flag    �������ԣ������MSDN
	virtual void OnKeyUp(fuInt KeyCode, fuInt Flag) {}

	/// @brief     �ַ�����
	/// @param[in] CharCode �ַ�
	/// @param[in] Flag     �������ԣ������MSDN
	virtual void OnCharInput(fCharW CharCode, fuInt Flag) {}

	/// @brief ���뷨��ʼƴд
	virtual void OnIMEStartComposition() {}

	/// @brief ���뷨����ƴд
	virtual void OnIMEEndComposition() {}

	/// @brief     ���뷨����ƴд
	/// @param[in] String   ƴд�����ı�ָ�룬����ɴ��ڽӿڻ��
	/// @param[in] CharCode ������ַ�
	virtual void OnIMEComposition(fcStrW String, fCharW CharCode) {}

	/// @brief     ���뷨����
	/// @param[in] Desc ���뷨����������ɴ��ڽӿڻ��
	virtual void OnIMEActivated(fcStrW Desc) {}

	/// @brief ���뷨�ر�
	virtual void OnIMEClosed() {}

	/// @brief     ���뷨��ѡ�ʸı�
	/// @param[in] pList ��ѡ���б�����ɴ��ڽӿڻ��
	virtual void OnIMEChangeCandidate(f2dIMECandidateList* pList) {}

	/// @brief ���뷨��ѡ�ʴ��ڴ�
	/// @param[in] pList ��ѡ���б�����ɴ��ڽӿڻ��
	virtual void OnIMEOpenCandidate(f2dIMECandidateList* pList) {}

	/// @brief ���뷨��ѡ�ʴ��ڴ�
	/// @param[in] pList ��ѡ���б�����ɴ��ڽӿڻ��
	virtual void OnIMECloseCandidate(f2dIMECandidateList* pList) {}

	/// @brief     ����ƶ�
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseMove(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ���ֹ���
	/// @param[in] X     ������λ��
	/// @param[in] Y     �������λ��
	/// @param[in] Wheel ���ֹ�������
	/// @param[in] Flag  �������ԣ������MSDN
	virtual void OnMouseWheel(fShort X, fShort Y, fFloat Wheel, fuInt Flag) {}

	/// @brief     ����������
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseLBDown(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     �������ſ�
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseLBUp(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ������˫��
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseLBDouble(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����м�����
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseMBDown(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����м��ſ�
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseMBUp(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����м�˫��
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseMBDouble(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����Ҽ�����
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseRBDown(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����Ҽ��ſ�
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseRBUp(fShort X, fShort Y, fuInt Flag) {}

	/// @brief     ����Ҽ�˫��
	/// @param[in] X    ������λ��
	/// @param[in] Y    �������λ��
	/// @param[in] Flag �������ԣ������MSDN
	virtual void OnMouseRBDouble(fShort X, fShort Y, fuInt Flag) {}

	/// @brief ��ý���
	virtual void OnGetFocus() {}

	/// @brief ��ʧ����
	virtual void OnLostFocus() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D��Ⱦ����
////////////////////////////////////////////////////////////////////////////////
struct f2dWindow
{
	/// @brief ���ؼ�����
	virtual f2dWindowEventListener* GetListener()=0;

	/// @brief ���ü�����
	virtual fResult SetListener(f2dWindowEventListener* pListener)=0;

	/// @brief   ����Windows���ھ��
	/// @note    ��Ҫǿת��HWND
	/// @warning �����������Ӧ������ʹ�ñ�����
	virtual fInt GetHandle()=0;

	/// @brief ���ر߿�����
	virtual F2DWINBORDERTYPE GetBorderType()=0;

	/// @brief ���ñ߿�����
	virtual fResult SetBorderType(F2DWINBORDERTYPE Type)=0;

	/// @brief ���ش��ڱ���
	virtual fcStrW GetCaption()=0;

	/// @brief ���ô��ڱ���
	/// @note  ������1024���ַ�
	virtual fResult SetCaption(fcStrW Caption)=0;

	/// @brief �����Ƿ�ɼ�
	virtual fBool GetVisiable()=0;

	/// @brief �����Ƿ�ɼ�
	virtual fResult SetVisiable(fBool Visiable)=0;

	/// @brief ���ش���λ�ã����߿�
	virtual fcyRect GetRect()=0;

	/// @brief ���ô���λ�ã����߿�
	virtual fResult SetRect(const fcyRect& Range)=0;

	/// @brief ���ش��ڿͻ���λ��
	virtual fcyRect GetClientRect()=0;

	/// @brief ���ô��ڿͻ���λ��
	virtual fResult SetClientRect(const fcyRect& Range)=0;

	/// @brief �ƶ�����Ļ����
	virtual void MoveToCenter()=0;

	/// @brief �Ƿ��ö�
	virtual fBool IsTopMost()=0;

	/// @brief �����ö�
	virtual fResult SetTopMost(fBool TopMost)=0;
	
	/// @brief     �������
	/// @param[in] bHide �Ƿ��������
	virtual void HideMouse(fBool bHide)=0;

	/// @brief ����IME
	virtual fBool IsHideIME()=0;
	
	/// @brief �����Ƿ�����IME
	virtual void SetHideIME(fBool bHide)=0;

	/// @brief ����IME����
	virtual fcStrW GetIMEDesc()=0;

	/// @brief ����IME��Ϣ
	virtual fuInt GetIMEInfo(F2DIMEINFO InfoType)=0;

	/// @brief ���ص�ǰ��ƴд��
	virtual fcStrW GetIMECompString()=0;

	/// @brief ���غ�ѡ�ʸ���
	virtual fuInt GetIMECandidateCount()=0;

	/// @brief ���غ�ѡ��
	virtual fcStrW GetIMECandidate(fuInt Index)=0;
};

/// @}
