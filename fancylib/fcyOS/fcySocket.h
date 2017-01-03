////////////////////////////////////////////////////////////////////////////////
/// @file  fcySocket.h
/// @brief fancy Socket��װ
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#	define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include "../fcyType.h"
#include "../fcyException.h"

#include <vector>
#include <string>

/// @addtogroup fancy��ײ�֧��
/// @{

/// @brief Socket��װģ��
namespace fcySocket
{
	/// @brief ��ʼ���ײ�Socket��
	fResult InitSocket();

	/// @brief �ر�Socket��
	void HaltSocket();

	/// @brief ����תIP
	std::string HostToIp(fcStr Host);

	/// @brief ����תIP
	std::vector<std::string> HostToAllIp(fcStr Host);

	/// @brief �쳣
	class Exception :
		public fcyException
	{
	private:
		fuInt m_WSALastError;          ///< @brief ���һ�δ������
	public:
		fuInt GetWSAErrorCode()const;  ///< @brief ������һ�δ���
	public:
		Exception(fcStr Src, fcStr Desc);
		~Exception();
	};

	/// @brief �ͻ���
	class Client
	{
	protected:
		void* m_Socket;
	protected:
		/// @brief     �ڲ����������ӵ�IP
		/// @exception fcySocket::Exception
		void connectTo(fcStr IP, fuShort Port);
	public:
		/// @brief     �������ݵ�������
		/// @note      ����ֱ������������
		/// @exception fcySocket::Exception
		/// @param[in] pBuffer ���ݻ�����
		/// @param[in] Length  ���ݳ���
		fuInt Recv(fData pBuffer, fLen Length);
		/// @brief     ��������
		/// @exception fcySocket::Exception
		/// @param[in] pBuffer ���ݻ�����
		/// @param[in] Length  ���ݳ���
		void Send(fcData pBuffer, fLen Length);
		/// @brief     ����һ���ֽ�����
		/// @exception fcySocket::Exception
		fChar RecvChar();
		/// @brief     ����һ���ֽ�����
		/// @exception fcySocket::Exception
		/// @param[in] Data ����
		void SendChar(fChar Data);
	public:
		/// @brief     ���캯��
		/// @exception fcySocket::Exception
		/// @param[in] IP   IP��ַ
		/// @param[in] Port �˿�
		Client(fcStr IP, fuShort Port);
		~Client();
	};
};

/// @}
