////////////////////////////////////////////////////////////////////////////////
/// @file  fcyStopWatch.h
/// @brief fancy��ʱ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyType.h"

/// @addtogroup fancy����
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief �߾���ͣ����
////////////////////////////////////////////////////////////////////////////////
class fcyStopWatch
{
private:
	fuLong m_cFreq;      ///< @brief CPUƵ��
	fuLong m_cLast;      ///< @brief ��һ��ʱ��
	fuLong m_cFixStart;  ///< @brief ��ͣʱ��ʱ���޸�����
	fuLong m_cFixAll;    ///< @brief ��ͣʱ��ʱ���޸�����
public:
	void Pause();        ///< @brief ��ͣ
	void Resume();       ///< @brief ����
	void Reset();        ///< @brief ����
	fDouble GetElapsed(); ///< @brief �������ʱ��
	                      ///< @note  ����Ϊ��λ
public:
	fcyStopWatch();
	~fcyStopWatch();
};
/// @}
