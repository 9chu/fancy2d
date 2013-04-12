////////////////////////////////////////////////////////////////////////////////
/// @file  f2dVideoImpl.h
/// @brief fancy2D视频对象实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dVideoStream.h"

#include "fcyMisc/fcyStopWatch.h"

#include "../f2dRenderer.h"
#include "../f2dVideoSys.h"

#include <Dvdmedia.h>

class f2dVideoSysImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D视频渲染到纹理
////////////////////////////////////////////////////////////////////////////////
class f2dVideoRenderer : 
	public CBaseVideoRenderer
{
private:
	// {598256BC-185B-4F8C-9881-DA12AB50CC74}
	static const GUID GUID_f2dVideoRenderer;
	enum SUPPORTMEDIATYPE
	{
		SUPPORTMEDIATYPE_RGB24,
		SUPPORTMEDIATYPE_RGB32,
		SUPPORTMEDIATYPE_ARGB32,
		SUPPORTMEDIATYPE_YUY2
	};
private:
	fInt m_TexWidth;                 // 纹理宽度
	fInt m_TexHeight;                // 纹理高度

	SUPPORTMEDIATYPE m_RawDataType;  // 原始格式
	fuInt m_lVideoWidth;             // 视频宽度
	fuInt m_lVideoHeight;            // 视频高度
	fuInt m_lVideoPitch;             // 视频数据补齐值

	CCritSec m_Lock;                 // 锁

	// 缓冲区
	fInt m_BufferFlag;           // 缓冲区标识，标记正在使用的缓冲区
	fcyColor* m_TexBuffer[2];    // 纹理数据数组
	CCritSec m_BufferLock[2];    // 缓冲区锁

	f2dVideoRendererListener* m_pListener; // 监听器

	// 吞吐量计算
	fcyStopWatch m_Watch;
	fFloat m_TimeTotal;
	fLen m_DataTotal;
	fLen m_FPSTotal;
	fFloat m_DataPerSecond;
	fFloat m_FPS;
private:
	FCYINLINE fByte clip255(fInt value)
	{
		if(value < 0)
			return 0;
		else if(value > 255)
			return 255;
		return (fByte)value;
	}

	void renderRGBSample(fcData pVideo, fcyColor* pTex);
	void renderYUY2Sample(fcData pVideo, fcyColor* pTex);
public:
	f2dVideoRendererListener* GetRendererListener();
	void SetRendererListener(f2dVideoRendererListener* pListener);
	fcyColor* GetInternalPtr(fuInt Index);
	fuInt GetVideoWidth();
	fuInt GetVideoHeight();
	fResult CopyDataToTexture(f2dTexture* pTex);
	fFloat GetDataPerSecond();
	fFloat GetVideoRenderFPS();
public: // 接口实现
	HRESULT CheckMediaType(const CMediaType *pMediaType);
	HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT DoRenderSample(IMediaSample *pMediaSample);
public:
	f2dVideoRenderer(HRESULT *phr = NULL);
	~f2dVideoRenderer();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D视频采样器信息
////////////////////////////////////////////////////////////////////////////////
struct f2dVideoFilterInfo
{
	HINSTANCE hDLLInstance; ///< @brief DLL实例
	GUID FilterGUID;        ///< @brief 过滤器GUID
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D视频对象实现
////////////////////////////////////////////////////////////////////////////////
class f2dVideoImpl :
	public fcyRefObjImpl<f2dVideo>
{
private:
	// --- 视频流对象 ---
	f2dVideoStream* m_pStream;
	f2dVideoStreamReader* m_pReader;

	// --- DEBUG用 ---
	DWORD m_ObjectTableEntry;

	// --- DShow组件 ---
	IGraphBuilder* m_pFilterGraph;  ///< @brief 过滤器管理器
	IMediaControl* m_pMediaControl; ///< @brief 媒体控制器
	IMediaEventEx* m_pMediaEvent;   ///< @brief 媒体事件
	IMediaSeeking* m_pMediaSeek;    ///< @brief 媒体寻址
	IVideoFrameStep* m_pVideoStep;  ///< @brief 视频单步渲染控制
	IBasicAudio* m_pAudioControl;   ///< @brief 声音控制

	// --- 纹理组件 ---
	f2dVideoRenderer* m_pRenderer;
	f2dTexture* m_pTex;

	// --- 状态 ---
	fBool m_bLoop;
private:
	void clearUp();
	void registerDebug();
	void unregisterDebug();
private:
	fBool addFilter(const f2dVideoFilterInfo& Filter);
	void clearUselessFilter();
public: // 接口实现
	f2dVideoRendererListener* GetRendererListener();
	void SetRendererListener(f2dVideoRendererListener* pListener);

	fResult Update();
	f2dTexture* GetVideoTex();

	fuInt GetVideoWidth();
	fuInt GetVideoHeight();
	fFloat GetVideoDataRate();
	fFloat GetVideoRenderFPS();
	fResult CopyDataToTexture(f2dTexture* pTex);

	fBool IsLoop();
	void SetLoop(fBool bLoop);
	F2DVIDEOSTATE GetState();
	fResult Play();
	fResult Pause();
	fResult Stop();

	fDouble GetTotalTime();
	fDouble GetCurTime();
	fResult SetCurTime(fDouble Time);
	fResult Seek(fuInt KeyFrame);
	fDouble GetRate();
	fResult SetRate(fDouble Rate);
	fFloat  GetVolume();
	fResult SetVolume(fFloat Volume);
	fFloat  GetPan();
	fResult SetPan(fFloat Pan);
public:
	f2dVideoImpl(f2dVideoSysImpl* pParent, f2dStream* pStream, fuInt Flag);
	~f2dVideoImpl(void);
};

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D视频系统实现
////////////////////////////////////////////////////////////////////////////////
class f2dVideoSysImpl :
	public fcyRefObjImpl<f2dVideoSys>
{
	friend class f2dVideoImpl;
private:
	f2dEngineImpl* m_pEngine;

	f2dRenderDevice* m_pDev;
	std::vector<f2dVideoFilterInfo> m_FliterGUIDList;
private:
	GUID strToGUID(fcStrW pGUIDStr);
	static BOOL createInstanceFromDll(HINSTANCE hDll, REFCLSID rclsid, REFIID riid, LPVOID * ppv);
public:
	f2dRenderDevice* GetRenderDevice();
public: // 接口实现
	fResult LoadFilter(fcStrW pGUIDStr);
	fResult LoadFilterFromAX(fcStrW AXFilePath, fcStrW pGUIDStr);
	fResult CreateVideo(f2dStream* pStream, fuInt Flag, f2dVideo** pOut);
public:
	f2dVideoSysImpl(f2dEngineImpl* pEngine);
	~f2dVideoSysImpl();
};
