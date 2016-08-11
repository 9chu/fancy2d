////////////////////////////////////////////////////////////////////////////////
/// @file  f2dFPSControllerImpl.h
/// @brief fancy2D FPS������
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyIO/fcyStream.h>
#include <fcyMisc/fcyStopWatch.h>

#include "../f2dEngine.h"

#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
/// @brief FPS������ʵ��
////////////////////////////////////////////////////////////////////////////////
class f2dFPSControllerImpl : 
	public f2dFPSController
{
protected:
	// ������
	fuInt m_TotalFrame;      ///< @brief ��֡��
	fDouble m_TotalTime;     ///< @brief ������ʱ��
	fDouble m_MaxFPS;        ///< @brief ���FPS
	fDouble m_CurFPS;        ///< @brief ��ǰFPS

	// ��֡������
	fDouble m_FrameTimeSecond; ///< @brief ����1��
	fuInt m_FrameCountSecond;  ///< @brief 1����֡��

	fuInt m_FPSMax;       ///< @brief ���FPS
	fDouble m_FrameDelay; ///< @brief ֡ʱ����
public: // ����
	/// @brief ����FPS������
	/// @note  �������ʱ���֮�������ʱ�䲢����FPS������
	fDouble Update(fcyStopWatch& Watch)
	{
		fDouble tElapsedTime = Watch.GetElpased();

		// FPS����
		if(m_FPSMax && tElapsedTime < m_FrameDelay)
		{
			fuInt tTimeToSleep = (fuInt)((m_FrameDelay - tElapsedTime) * 1000.);

			// Sleep����
			if(tTimeToSleep > 2)
				Sleep(tTimeToSleep - 2);

			// ������ȷ����
			while((tElapsedTime = Watch.GetElpased()) < m_FrameDelay) {}
		}

		tElapsedTime = Watch.GetElpased();
		Watch.Reset();

		m_TotalFrame++;
		m_TotalTime += tElapsedTime;

		m_FrameTimeSecond += tElapsedTime;
		m_FrameCountSecond++;

		// FPS����
		if(m_FrameTimeSecond >= 1.)
		{
			m_CurFPS = m_FrameCountSecond / m_FrameTimeSecond;
			m_FrameCountSecond = 0;
			m_FrameTimeSecond = 0.;

			if(m_CurFPS > m_MaxFPS)
				m_MaxFPS = m_CurFPS;
		}

		return tElapsedTime;
	}
public: // �ӿ�ʵ��
	fuInt GetLimitedFPS()
	{
		return m_FPSMax;
	}
	void SetLimitedFPS(fuInt iMaxFPS)
	{
		m_FPSMax = iMaxFPS;
		m_FrameDelay = m_FPSMax ? 1./m_FPSMax : 0;
	}
	fDouble GetFPS()
	{
		return m_CurFPS;
	}
	fuInt GetTotalFrame()
	{
		return m_TotalFrame;
	}
	fDouble GetTotalTime()
	{
		return m_TotalTime;
	}
	fDouble GetAvgFPS()
	{
		if(m_TotalTime == 0)
			return 0;
		return m_TotalFrame / m_TotalTime;
	}
	fDouble GetMaxFPS()
	{
		return m_MaxFPS;
	}
public:
	f2dFPSControllerImpl(fuInt MaxFPS)
		: m_TotalFrame(0), m_TotalTime(0), m_MaxFPS(0), m_CurFPS(0),
		m_FrameTimeSecond(0), m_FrameCountSecond(0),
		m_FPSMax(MaxFPS), m_FrameDelay(MaxFPS?1./MaxFPS : 0) 
	{
		timeBeginPeriod(1);
	}
	~f2dFPSControllerImpl()
	{
		timeEndPeriod(1);
	}
};
