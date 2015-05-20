////////////////////////////////////////////////////////////////////////////////
/// @file  fcyException.h
/// @brief fancy���쳣����
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyType.h"

#include <string>

/// @addtogroup fancy���쳣
/// @brief ����fancy���е��쳣
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy�쳣����
/// @note  �����ڲ��쳣�Ӹ�������
////////////////////////////////////////////////////////////////////////////////
class fcyException
{
protected:
	fuInt m_Time;            ///< @brief ��¼�쳣ʱ���
	std::string m_ExcpSrc;   ///< @brief ��¼�쳣��Դ
	std::string m_ExcpDesc;  ///< @brief ��¼�쳣��Ϣ
public:
	fuInt GetTime()const;    ///< @brief ����쳣ʱ��
	fcStr GetSrc()const;     ///< @brief ����쳣��Դ
	fcStr GetDesc()const;    ///< @brief ����쳣��Ϣ
public:
	/// @brief     �չ��캯��
	fcyException();
	/// @brief     ���캯��
	/// @warning   �������ó���512�ַ�
	/// @param[in] Src     �쳣Դ
	/// @param[in] DescFmt �쳣��ʽ
	fcyException(fcStr Src, fcStr DescFmt, ...);
	fcyException(const fcyException& Org);
	~fcyException();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcyWin32�쳣
/// @note  ��װWin32�쳣
////////////////////////////////////////////////////////////////////////////////
class fcyWin32Exception :
	public fcyException
{
private:
	fuInt m_LastError;          ///< @brief ���һ�δ������
public:
	fuInt GetErrorCode()const;  ///< @brief ������һ�δ���
public:
	/// @brief     ���캯��
	/// @note      �쳣���Զ���Info�ֶ�׷��LastErrorֵ
	/// @param[in] Src  �쳣Դ
	/// @param[in] Info �쳣����
	fcyWin32Exception(fcStr Src, fcStr Info);
	~fcyWin32Exception();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcyWin32COM�쳣
/// @note  ��װCOM�쳣
////////////////////////////////////////////////////////////////////////////////
class fcyWin32COMException :
	public fcyException
{
private:
	fInt m_HR;              ///< @brief ����HResult�ֶ�
public:
	fInt GetResult()const;  ///< @brief ����HResult�ֶ�
public:
	/// @brief     ���캯��
	/// @note      �쳣���Զ���Info�ֶ�׷��HRESULTֵ
	/// @param[in] Src  �쳣Դ
	/// @param[in] Info �쳣����
	/// @param[in] HRet HResult
	fcyWin32COMException(fcStr Src, fcStr Info, fInt HRet);
	~fcyWin32COMException();
};

/// @}