////////////////////////////////////////////////////////////////////////////////
/// @file  f2dSoundSysImpl.h
/// @brief fancy2D音频系统接口实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyRefObj.h"
#include "fcyIO\fcyStream.h"
#include "fcyOS/fcyMultiThread.h"

#include "f2dSoundDecoder.h"

#include <deque>

#include <dsound.h>

////////////////////////////////////////////////////////////////////////////////
/// @brief 动态加载dSound
////////////////////////////////////////////////////////////////////////////////
class f2dSoundSysAPI
{
private:
	typedef HRESULT (WINAPI *pDLLEntry_DirectSoundCreate8)(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);
private:
	HMODULE m_hModule;

	pDLLEntry_DirectSoundCreate8 m_Entry_pDirectSoundCreate8;
public:
	HRESULT DLLEntry_DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);
public:
	f2dSoundSysAPI();
	~f2dSoundSysAPI();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 静态声音缓冲
////////////////////////////////////////////////////////////////////////////////
class f2dSoundBufferStatic :
	public fcyRefObjImpl<f2dSoundBuffer>
{
	friend class f2dSoundSysImpl;
protected:
	IDirectSoundBuffer* m_pBuffer; ///< @brief 声音缓冲
	
	fBool m_bLoop;      ///< @brief 是否循环
	fuInt m_BufferSize; ///< @brief 缓冲区大小
	fuInt m_psSize;     ///< @brief 一秒钟所占字节数
public: // 接口实现
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
protected: // 禁止直接new/delete
	f2dSoundBufferStatic(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus);
	~f2dSoundBufferStatic();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 动态声音缓冲
/// @note  流式缓冲，使用预先准备的64kb的空间来播放声音
////////////////////////////////////////////////////////////////////////////////
class f2dSoundBufferDynamic :
	public fcyRefObjImpl<f2dSoundBuffer>
{
	friend class f2dSoundSysImpl;
private:
	static const fuInt BufferSize;  ///< @brief 缓冲区大小为64kb
	/// @brief 流同步线程
	class SoundBufferFiller :
		public fcyBaseThread
	{
	private:
		f2dSoundBufferDynamic* m_pParent;
	private:
		fuInt ThreadJob();
	public:
		SoundBufferFiller(f2dSoundBufferDynamic* pParent);
		~SoundBufferFiller();
	};
protected:
	// 接口
	IDirectSoundBuffer* m_pBuffer;
	IDirectSoundNotify* m_pNotify;
	f2dSoundDecoder* m_pDecoder;
	// 多线程
	SoundBufferFiller* m_pFiller;  ///< @brief 填充用线程
	fcyCriticalSection m_Sec;      ///< @brief 临界区
	fcyEvent m_EvtBegin;           ///< @brief 开头填充标志点
	fcyEvent m_EvtHalf;            ///< @brief 中途填充标志点
	fcyEvent m_EvtStop;            ///< @brief 终止位置
	fBool m_bHalt;                 ///< @brief 线程终止消息
	// 播放数据
	fuInt m_StartPos;        ///< @brief 播放开始位置
	fInt  m_PlayTime;        ///< @brief 播放时间
	fBool m_bLoop;           ///< @brief 循环标志
	fuInt m_psSize;          ///< @brief 单采样大小
	fuInt m_BufferSize;      ///< @brief 缓冲大小
private:
	void preInit(fuInt StartPos);  ///< @brief 【线程安全】初始化并填充缓冲区
	void updateTime();             ///< @brief 【非线程安全】更新时间
	void fillBuffer(fuInt Index);  ///< @brief 【非线程安全】从内部缓冲区填充音频缓冲区
	void regNotify();              ///< @brief 【非线程安全】注册监听
	void regStopNotify(fuInt Pos); ///< @brief 【非线程安全】注册停止
	fBool isThreadHalt();          ///< @brief 【线程安全】判断线程退出事件
	void haltThread();             ///< @brief 【线程安全】安全终止线程
public: // 接口实现
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
protected: // 禁止直接new/delete
	f2dSoundBufferDynamic(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus);
	~f2dSoundBufferDynamic();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 音效精灵
////////////////////////////////////////////////////////////////////////////////
class f2dSoundSpriteImpl : 
	public fcyRefObjImpl<f2dSoundSprite>
{
	friend class f2dSoundSysImpl;
private:
	IDirectSound8* m_pParent;
	IDirectSoundBuffer* m_pBuffer;                 ///< @brief 原始缓冲
	std::deque<IDirectSoundBuffer*> m_BufferList;  ///< @brief 使用中缓冲
	fuInt m_BufferSize;
	fuInt m_psSize;
public: // 接口实现
	void Play();
	void Stop();
	void Pause();
	fDouble GetTotalTime();

	fResult PlayNewSound(fFloat Volume, fFloat Pan);
protected: // 禁止直接new/delete
	f2dSoundSpriteImpl(IDirectSound8* pSound, f2dSoundDecoder* pDecoder, fBool bGlobalFocus);
	~f2dSoundSpriteImpl();
};

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief 声音系统实现
////////////////////////////////////////////////////////////////////////////////
class f2dSoundSysImpl :
	public fcyRefObjImpl<f2dSoundSys>
{
private:
	f2dEngineImpl* m_pEngine;

	f2dSoundSysAPI m_API;

	IDirectSound8* m_pDSound8;
public:
	fResult CreateStaticBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut);
	fResult CreateDynamicBuffer(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundBuffer** pOut);
	fResult CreateSoundSprite(f2dSoundDecoder* pDecoder, fBool bGlobalFocus, f2dSoundSprite** pOut);

	fResult CreateWaveDecoder(f2dStream* pStream, f2dSoundDecoder** pOut);
	fResult CreateOGGVorbisDecoder(f2dStream* pStream, f2dSoundDecoder** pOut);
public:
	f2dSoundSysImpl(f2dEngineImpl* pEngine);
protected:
	~f2dSoundSysImpl();
};
