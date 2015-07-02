////////////////////////////////////////////////////////////////////////////////
/// @file  f2dSoundBufferPull.h
/// @brief fancy2D��Ƶϵͳ ��̬������������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyRefObj.h"
#include "fcyIO\fcyStream.h"
#include "fcyOS/fcyMultiThread.h"

#include "..\f2dSoundSys.h"

#include <dsound.h>

////////////////////////////////////////////////////////////////////////////////
/// @brief ���ڶ�̬���������������
////////////////////////////////////////////////////////////////////////////////
class f2dSoundBufferPull :
	public fcyRefObjImpl<f2dSoundBuffer>
{
	friend class f2dSoundSysImpl;
private:
	/// @brief ��ͬ���߳�
	class SoundBufferFiller :
		public fcyBaseThread
	{
	private:
		f2dSoundBufferPull* m_pParent;
	private:
		fuInt ThreadJob();
	public:
		SoundBufferFiller(f2dSoundBufferPull* pParent);
		~SoundBufferFiller();
	};
protected:
	// �ӿ�
	IDirectSoundBuffer* m_pBuffer;
	IDirectSoundNotify* m_pNotify;
	f2dSoundDecoder* m_pDecoder;
	// ���߳�
	SoundBufferFiller* m_pFiller;  ///< @brief ������߳�
	fcyCriticalSection m_Sec;      ///< @brief �ٽ���
	fcyEvent m_EvtBegin;           ///< @brief ��ͷ����־��
	fcyEvent m_EvtHalf;            ///< @brief ��;����־��
	fcyEvent m_EvtStop;            ///< @brief ��ֹλ��
	fBool m_bHalt;                 ///< @brief �߳���ֹ��Ϣ
	// ��������
	fuInt m_iBufferSize;     ///< @brief ��������С
private:
	void preInit(fuInt StartPos);  ///< @brief ���̰߳�ȫ����ʼ������仺����
	void updateTime();             ///< @brief �����̰߳�ȫ������ʱ��
	void fillBuffer(fuInt Index);  ///< @brief �����̰߳�ȫ�����ڲ������������Ƶ������
	void regNotify();              ///< @brief �����̰߳�ȫ��ע�����
	void regStopNotify(fuInt Pos); ///< @brief �����̰߳�ȫ��ע��ֹͣ
	fBool isThreadHalt();          ///< @brief ���̰߳�ȫ���ж��߳��˳��¼�
	void haltThread();             ///< @brief ���̰߳�ȫ����ȫ��ֹ�߳�
public: // �ӿ�ʵ��
	fBool IsDynamic();

	void Play();
	void Stop();
	void Pause();

	fBool IsLoop();
	void SetLoop(fBool bValue);
	fBool IsPlaying();
	fDouble GetTotalTime();
	fDouble GetTime();
	fResult SetTime(fDouble Time);
	fFloat GetVolume();
	fResult SetVolume(fFloat Value);
	fFloat GetPan();
	fResult SetPan(fFloat Value);
	fuInt GetFrequency();
	fResult SetFrequency(fuInt Value);
protected: // ��ֱֹ��new/delete
	f2dSoundBufferPull(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fuInt iBufferSampleCount, fBool bGlobalFocus);
	~f2dSoundBufferPull();
};
