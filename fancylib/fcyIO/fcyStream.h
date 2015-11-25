////////////////////////////////////////////////////////////////////////////////
/// @file  fcyStream.h
/// @brief ������ʵ����fancy�ڲ�����ʽ�ӿ�
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyRefObj.h"
#include "../fcyException.h"
#include "../fcyOS/fcyMultiThread.h"

#include <string>
#include <vector>

#define FCYSTREAM

/// @addtogroup fancy��IOģ��
/// @brief      �ṩ���ļ����ڴ�Ķ�д�Լ��������֧��
/// @note       ����fcyStream��IOϵͳ��ͨ��ʵ��fcyStream�ӿ���ʵ��������Դ����
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy��Ѱַ��ʽ
////////////////////////////////////////////////////////////////////////////////
enum FCYSEEKORIGIN
{
	FCYSEEKORIGIN_BEG = 0,  ///< @brief ��ͷ��ʼѰַ
	                        ///< @note  ָ��Ѱַλ�ÿ�ʼ��0��
	FCYSEEKORIGIN_CUR = 1,  ///< @brief �ӵ�ǰλ�ÿ�ʼѰַ
	FCYSEEKORIGIN_END = 2   ///< @brief �ӽ�β����ʼѰַ
	                        ///< @note  ָ��Ѱַλ�ÿ�ʼ��EOF��
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy���ӿ�
////////////////////////////////////////////////////////////////////////////////
struct fcyStream : fcyRefObj
{
	/// @brief �������Ƿ��д
	virtual fBool CanWrite()=0;

	/// @brief �������Ƿ�ɱ䳤
	virtual fBool CanResize()=0;

	/// @brief ����������
	virtual fLen GetLength()=0;

	/// @brief     �����³���
	/// @param[in] Length �����³���
	virtual fResult SetLength(fLen Length)=0;
	
	/// @brief ��ö�дָ���λ��
	virtual fLen GetPosition()=0;

	/// @brief     ���ö�дλ��
	/// @param[in] Origin Ѱַ�ο�λ��
	/// @param[in] Offset λ��
	virtual fResult SetPosition(FCYSEEKORIGIN Origin, fLong Offset)=0;

	/// @brief      �����ж�ȡ�ֽ�����
	/// @param[in]  pData      Ŀ�Ļ�����
	/// @param[in]  Length     ���ݳ���
	/// @param[out] pBytesRead ��ʵ��д���ȣ�����ΪNULL
	virtual fResult ReadBytes(fData pData, fLen Length, fLen* pBytesRead=NULL)=0;

	/// @brief      ������д���ֽ�����
	/// @param[in]  pSrc        ԭʼ������
	/// @param[in]  Length      ���ݳ���
	/// @param[out] pBytesWrite ��ʵ��д���ȣ�����ΪNULL
	virtual fResult WriteBytes(fcData pSrc, fLen Length, fLen* pBytesWrite=NULL)=0;

	/// @brief   ������
	/// @note    �ú������ܻ��������
	/// @warning ���һ�����ڶ��̻߳����±�ʹ��ʱ�����ڶ�д�������ֶ�����
	virtual void Lock()=0;
	
	/// @brief   ��ͼ������
	/// @warning ���һ�����ڶ��̻߳����±�ʹ��ʱ�����ڶ�д�������ֶ�����
	/// @return  ʹ��FCYOK��FCYFAILED�ж��Ƿ�ɹ�����
	virtual fResult TryLock()=0;

	/// @brief   ������
	/// @note    �ú���������Lock��TryLock�ɹ��������½��е���
	/// @warning ���һ�����ڶ��̻߳����±�ʹ��ʱ�����ڶ�д�������ֶ�����
	virtual void Unlock()=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy�ļ���ʵ��
////////////////////////////////////////////////////////////////////////////////
class fcyFileStream :
	public fcyRefObjImpl<fcyStream>
{
private:
	HANDLE m_hFile;       ///< @brief �ڲ��ļ����
	fBool m_bWritable;    ///< @brief �Ƿ��д
	std::wstring m_sPath; ///< @brief �ļ�·��
	fcyCriticalSection m_CriticalSec; ///< @brief �ٽ���
public: // �ӿ�ʵ��
	fBool CanWrite();
	fBool CanResize();
	fLen GetLength();
	fResult SetLength(fLen Length);
	fLen GetPosition();
	fResult SetPosition(FCYSEEKORIGIN Origin, fLong Offset);
	fResult ReadBytes(fData pData, fLen Length, fLen* pBytesRead);
	fResult WriteBytes(fcData pSrc, fLen Length, fLen* pBytesWrite);
	void Lock();
	fResult TryLock();
	void Unlock();
public:
	/// @brief     ���캯��
	/// @param[in] Path     �ļ�·��
	/// @param[in] Writable ��д
	fcyFileStream(fcStrW Path, fBool Writable);
protected:
	~fcyFileStream();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy�ڴ���
////////////////////////////////////////////////////////////////////////////////
class fcyMemStream :
	public fcyRefObjImpl<fcyStream>
{
private:
	std::vector<fByte> m_Data;        ///< @brief �ڲ�����
	fBool m_bResizable;               ///< @brief �ɱ䳤
	fBool m_bWritable;                ///< @brief ��д
	fLen m_cPointer;                  ///< @brief ��дλ��
	fcyCriticalSection m_CriticalSec; ///< @brief �ٽ���
public: // �ӿ�ʵ��
	fBool CanWrite();
	fBool CanResize();
	fLen GetLength();
	fResult SetLength(fLen Length);
	fLen GetPosition();
	fResult SetPosition(FCYSEEKORIGIN Origin, fLong Offset);
	fResult ReadBytes(fData pData, fLen Length, fLen* pBytesRead);
	fResult WriteBytes(fcData pSrc, fLen Length, fLen* pBytesWrite);
	void Lock();
	fResult TryLock();
	void Unlock();
public: // ��չ�ӿ�
	fData GetInternalBuffer();
public:
	/// @brief     ���캯��
	/// @param[in] Src       ����Դ����ΪNULL�򲻴�����Դ����
	/// @param[in] Length    ���ݳ���
	/// @param[in] Writable  ��д
	/// @param[in] Resizable �ɱ䳤
	fcyMemStream(fcData Src, fLen Length, fBool Writable, fBool Resizable);
protected:
	~fcyMemStream();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy������
/// @note  ������һ������һ���ֽ��з���
////////////////////////////////////////////////////////////////////////////////
class fcyPartialStream :
	public fcyRefObjImpl<fcyStream>
{
private:
	fcyStream* m_pOrgStream; ///< @brief ԭʼ��
	fLen m_Offset;           ///< @brief ��ԭʼ���е�ƫ����
	fLen m_pPos;             ///< @brief ��ǰ��дλ��
	fLen m_Len;              ///< @brief ԭʼ������
public: // �ӿ�ʵ��
	fBool CanWrite();
	fBool CanResize();
	fLen GetLength();
	fResult SetLength(fLen Length);
	fLen GetPosition();
	fResult SetPosition(FCYSEEKORIGIN Origin, fLong Offset);
	fResult ReadBytes(fData pData, fLen Length, fLen* pBytesRead);
	fResult WriteBytes(fcData pSrc, fLen Length, fLen* pBytesWrite);
	void Lock();
	fResult TryLock();
	void Unlock();
public:
	/// @brief     ���캯��
	/// @param[in] OrgStream ԭʼ��ָ��
	/// @param[in] Offset    ��������ԭʼ���е�λ��
	/// @param[in] Size      ��������С
	fcyPartialStream(fcyStream* OrgStream, fLen Offset, fLen Size);
protected:
	~fcyPartialStream();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fcy������
////////////////////////////////////////////////////////////////////////////////
namespace fcyStreamHelper
{
	/// @brief     �����
	/// @note      ��Src�ĵ�ǰλ�ö�ȡDataLength�����ȵ�Dest����
	/// @param[in] Src ԭʼ��
	/// @param[in] Dest Ŀ����
	/// @param[in] DataLength ���ݳ���
	/// @return    FCYERR_OK�������ɹ���ɣ�FCYERR_INTERNALERR����ȡʱ����
	fResult FillStream(fcyStream* Src, fcyStream* Dest, fLen DataLength);
};
/// @}