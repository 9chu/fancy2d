////////////////////////////////////////////////////////////////////////////////
/// @file  fuiParticleEmitter.h
/// @brief fancyUI 粒子发射器
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 粒子发射器
////////////////////////////////////////////////////////////////////////////////
class fuiParticleEmitter :
	public fuiControl
{
protected: // 绘图资源
	fcyRefPointer<fuiSprite> m_pImageSprite;
	fcyRefPointer<fuiParticleDesc> m_pParticleDesc;

	f2dParticlePool* m_pPool;
	fDouble m_NextTime;
protected: // 属性
	std::wstring m_ParticleName;
	std::wstring m_ParticleSpriteName;
	fcyVec2 m_EmitterTimeRange;
	fcyVec2 m_EmitterCountRange;
	bool m_bEmitterEnable;

	fuiPropertyAccessor<std::wstring> m_Particle_Accessor;
	fuiPropertyAccessor<std::wstring> m_ParticleSprite_Accessor;
	fuiPropertyAccessor<fcyVec2> m_EmitterTimeRange_Accessor;
	fuiPropertyAccessor<fcyVec2> m_EmitterCountRange_Accessor;
	fuiPropertyAccessor<bool> m_bEmitterEnable_Accessor;
protected: // 监听事件
	void OnParentChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiParticleEmitter(fuiPage* pRootPage, const std::wstring& Name);
	~fuiParticleEmitter();
};
