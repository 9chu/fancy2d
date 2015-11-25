////////////////////////////////////////////////////////////////////////////////
/// @file  f2dSoundSys.h
/// @brief fancy2D����ϵͳ�ӿڶ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dInterface.h"

/// @addtogroup f2d����ϵͳ
/// @brief fancy2d��������ϵͳ
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief   ��Ƶ�������ӿ�
/// @warning һ��������ֻ������һ����Ƶ������һ�������Ա������������������
////////////////////////////////////////////////////////////////////////////////
struct f2dSoundDecoder : public f2dInterface
{
	// --- ����PCM���ݸ�ʽ ---
	virtual fuInt GetBufferSize()=0;        ///< @brief  ���ؽ����PCM�����ݴ�С
	virtual fuInt GetAvgBytesPerSec()=0;    ///< @brief  ���ÿ��ռ�õ��ֽ���
	virtual fuShort GetBlockAlign()=0;      ///< @brief  ��õ���������С
	virtual fuShort GetChannelCount()=0;    ///< @brief  ���������
	                                        ///< @return ����1������������2����������
	virtual fuInt GetSamplesPerSec()=0;     ///< @brief  ��ò�����
	                                        ///< @return ������44100
	virtual fuShort GetFormatTag()=0;       ///< @brief  ��ø�ʽ����
	                                        ///< @return ͨ������1
	virtual fuShort GetBitsPerSample()=0;   ///< @brief  ��ñ�����
	                                        ///< @return ������16��bits��

	// --- ������� ---
	/// @brief �������PCM���ݵĵ�ǰ����λ��
	virtual fLen GetPosition()=0;

	/// @brief     ���PCM���ݽ���Ѱַ
	/// @param[in] Origin Ѱַԭ��
	/// @param[in] Offset ƫ����
	virtual fResult SetPosition(F2DSEEKORIGIN Origin, fInt Offset)=0;

	/// @brief      ����PCM����
	/// @param[out] pBuffer    Ŀ�Ļ�����
	/// @param[in]  SizeToRead Ҫ��ȡ��PCM���ݴ�С
	/// @param[out] pSizeRead  ʵ�ʶ�ȡ��PCM���ݴ�С����ΪNULL
	virtual fResult Read(fData pBuffer, fuInt SizeToRead, fuInt* pSizeRead)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ����������
////////////////////////////////////////////////////////////////////////////////
struct f2dSoundBuffer : public f2dInterface
{
	virtual fBool IsDynamic()=0;          ///< @brief �Ƿ�Ϊ��̬������

	// --- ���Ų��� ---
	virtual void Play()=0;                ///< @brief ����
	virtual void Stop()=0;                ///< @brief ֹͣ
	virtual void Pause()=0;               ///< @brief ��ͣ

	// --- ���Կ��� ---
	virtual fBool IsLoop()=0;                    ///< @brief �����Ƿ�ѭ��
	virtual void SetLoop(fBool bValue)=0;        ///< @brief ��Ϊѭ��
	virtual fBool IsPlaying()=0;                 ///< @brief �����Ƿ����ڲ���
	virtual fDouble GetTotalTime()=0;            ///< @brief ����Ϊ��λ�����ܲ���ʱ��
	virtual fDouble GetTime()=0;                 ///< @brief ����Ϊ��λ���ز���λ��
	virtual fResult SetTime(fDouble Time)=0;     ///< @brief     ����Ϊ��λ���ò���λ��
		                                         ///< @param[in] Time ����λ��
	virtual fFloat GetVolume()=0;                ///< @brief  �������
	                                             ///< @return ����[0~1]��0��ʾ������1��ʾ��������
	virtual fResult SetVolume(fFloat Value)=0;   ///< @brief     ��������[0~1]
	                                             ///< @param[in] Value ����[0~1]��0��ʾ������1��ʾ��������
	virtual fFloat GetPan()=0;                   ///< @brief  �������ƽ��
	                                             ///< @return ����[-1~1]��-1��������0������1������
	virtual fResult SetPan(fFloat Value)=0;      ///< @brief     ��������ƽ��
	                                             ///< @param[in] Value ƽ�����[-1~1]��-1��������0������1������
	virtual fuInt GetFrequency()=0;              ///< @brief  ��ò���Ƶ��
	                                             ///< @return ����ÿ���Ӳ�������
	virtual fResult SetFrequency(fuInt Value)=0; ///< @brief     ���ò���Ƶ��
	                                             ///< @param[in] Value ȡֵ��Χ[100~100,000]
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ������Ƶ
/// @note  ���ڲ��Ŷ����ͬ��Ч
////////////////////////////////////////////////////////////////////////////////
struct f2dSoundSprite : public f2dInterface
{
	virtual void Play()=0;                ///< @brief ������������
	virtual void Stop()=0;                ///< @brief ֹͣ�����������൱�����
	virtual void Pause()=0;               ///< @brief ��ͣ��������
	virtual fDouble GetTotalTime()=0;     ///< @brief ���ԭʼ��������

	virtual fResult PlayNewSound(fFloat Volume = 1.f, fFloat Pan = 0.f)=0; ///< @brief ����һ������
};

////////////////////////////////////////////////////////////////////////////////
/// @brief ��Ƶϵͳ
////////////////////////////////////////////////////////////////////////////////
struct f2dSoundSys
{
	/// @brief      ������̬��Ƶ
	/// @note       ��̬��Ƶ�����������ص��ڴ��У��ʺ�������Ч
	/// @param[in]  pDecoder     ��Ƶ����������̬���������ᱣ�������ָ�룬���ڳ�ʼ��ʱ��������PCM����
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrue��Ӧ�ó��򲻱�����ʱ��Ȼ���Է�������
	/// @param[out] pOut         �������Ƶ������ָ��
	virtual fResult CreateStaticBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut)=0;

	/// @brief      ���������ڴ�ľ�̬��������Ƶ
	/// @note       �����ڴ滺�������ʺ��ظ����ŵ���Ч
	/// @param[in]  pOrg ԭʼ�ľ�̬������
	/// @param[out] pOut �����Ĺ����ڴ����εľ�̬������
	virtual fResult CreateSharedStaticBuffer(f2dSoundBuffer* pOrg, f2dSoundBuffer** pOut)=0;

	/// @brief      ������̬��Ƶ
	/// @note       ��̬��Ƶֻ�Ỻ�漫������PCM���ݣ�������룬�ʺϱ������֡�
	/// @param[in]  pDecoder     ��Ƶ����������̬�������ᱣ�������ָ�룬�벻Ҫ��һ�����������ڶ��������
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrue��Ӧ�ó��򲻱�����ʱ��Ȼ���Է�������
	/// @param[out] pOut         �������Ƶ������ָ��
	virtual fResult CreateDynamicBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut) = 0;

	/// @brief      ����������
	/// @note       ���������ڻ��������ֻ��Play��Pause��Ч�����ຯ������������
	/// @param[in]  pDecoder            �󶨵Ľ�����
	/// @param[in]  iBufferSampleCount  �������洢�Ĳ�����
	/// @param[in]  bGlobalFocus        ȫ����Ӧ����Ϊtrue��Ӧ�ó��򲻱�����ʱ��Ȼ���Է�������
	/// @param[out] pOut                �������Ƶ������ָ��
	virtual fResult CreatePullBuffer(f2dSoundDecoder* pDecoder, fuInt iBufferSampleCount, fBool bGlobalFocus, f2dSoundBuffer** pOut) = 0;

	/// @brief      ����������Ч
	/// @note       �ṩ��Ч���Ź��ܣ����PCM���ݶ�ȡ���ڴ���
	/// @param[in]  pDecoder     ��Ƶ����������̬���������ᱣ�������ָ�룬���ڳ�ʼ��ʱ��������PCM����
	/// @param[in]  bGlobalFocus ȫ����Ӧ����Ϊtrue��Ӧ�ó��򲻱�����ʱ��Ȼ���Է�������
	/// @param[in]  iMaxCount    ָ��ͬʱ���ŵ������Ч����, Ĭ�Ͽ�ȡ10
	/// @param[out] pOut         �������Ƶ������ָ��
	virtual fResult CreateSoundSprite(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, fuInt iMaxCount, f2dSoundSprite** pOut)=0;

	/// @brief      ����Wave������
	/// @note       ������������Ĵ�ȡ��Ϊ�������Ա�֤���ڶ��̻߳����±�����ʱ����������
	/// @param[in]  pStream ������
	/// @param[out] pOut    ����Ľ�����ָ��
	virtual fResult CreateWaveDecoder(f2dStream* pStream, f2dSoundDecoder** pOut)=0;

	/// @brief      ����Vorbis�����������ڽ���Ogg��Ƶ
	/// @note       ������������Ĵ�ȡ��Ϊ�������Ա�֤���ڶ��̻߳����±�����ʱ����������
	/// @param[in]  pStream ������
	/// @param[out] pOut    ����Ľ�����ָ��
	virtual fResult CreateOGGVorbisDecoder(f2dStream* pStream, f2dSoundDecoder** pOut)=0;
};

/// @}
