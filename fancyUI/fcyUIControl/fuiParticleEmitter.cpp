#include "fuiParticleEmitter.h"

#include "fuiExceptionMacro.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiParticleEmitter::fuiParticleEmitter(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_NextTime(0), m_pPool(NULL), m_bEmitterEnable(true)
{
	m_EmitterTimeRange = fcyVec2(1.f, 2.f);

	// 默认参数
	m_bClip = false;
	m_bMouseTrans = true;

	SetVisible(false);

	// 设置访问器
	m_Particle_Accessor = fuiPropertyAccessor<wstring>(
		&m_ParticleName,
		[&](std::wstring& Prop, const std::wstring* Value)
		{
			Prop = *Value;
		},
		[&](const std::wstring& Prop, std::wstring* Value)
		{
			*Value = Prop;
			ExecEvent(L"OnStyleChanged");
		}
	);
	m_ParticleSprite_Accessor = fuiPropertyAccessor<wstring>(
		&m_ParticleSpriteName,
		[&](std::wstring& Prop, const std::wstring* Value)
		{
			Prop = *Value;
		},
		[&](const std::wstring& Prop, std::wstring* Value)
		{
			*Value = Prop;
			ExecEvent(L"OnStyleChanged");
		}
	);
	m_EmitterTimeRange_Accessor = fuiPropertyAccessor<fcyVec2>(&m_EmitterTimeRange);
	m_EmitterCountRange_Accessor = fuiPropertyAccessor<fcyVec2>(&m_EmitterCountRange);
	m_bEmitterEnable_Accessor = fuiPropertyAccessor<bool>(&m_bEmitterEnable);

	// 设置属性
	RegisterProperty(L"Particle", &m_Particle_Accessor);
	RegisterProperty(L"ParticleSprite", &m_ParticleSprite_Accessor);
	RegisterProperty(L"EmitterTimeRange", &m_EmitterTimeRange_Accessor);
	RegisterProperty(L"EmitterCountRange", &m_EmitterCountRange_Accessor);
	RegisterProperty(L"EmitterEnable", &m_bEmitterEnable_Accessor);

	// 设置事件
	RegisterEvent(L"OnEmitter");

	// 设置事件回调
	GetEvent(L"OnParentChanged") += fuiDelegate::EventCallBack(this, &fuiParticleEmitter::OnParentChanged);
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiParticleEmitter::OnStyleChanged);
}

fuiParticleEmitter::~fuiParticleEmitter()
{
}

void fuiParticleEmitter::OnParentChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(GetParent() == NULL)
		throw fcyException("fuiParticleEmitter::OnParentChanged", "Invalid parent.");
	m_pPool = (f2dParticlePool*)GetParent()->GetProperty<void*>(L"ParticlePoolPtr");
}

void fuiParticleEmitter::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiParticleDesc> tParticleDesc;
	fcyRefPointer<fuiSprite> tImageSprite;

	FUIGETRESANDCHECK(m_ParticleName, tParticleDesc, fuiParticleDesc, fuiRes::RESTYPE_PARTICLE_DESC, "fuiParticleEmitter::OnStyleChanged");
	FUIGETRESANDCHECK(m_ParticleSpriteName, tImageSprite, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiParticleEmitter::OnStyleChanged");

	m_pParticleDesc = tParticleDesc;
	m_pImageSprite = tImageSprite;
}

void fuiParticleEmitter::Update(fDouble ElapsedTime)
{
	if(m_bEmitterEnable)
	{
		m_NextTime -= ElapsedTime;
		if(m_NextTime <= 0)
		{
			ExecEvent(L"OnEmitter");

			// 发射粒子
			if(m_pPool && m_pImageSprite && m_pParticleDesc)
			{
				m_pPool->Emitted(m_pImageSprite->GetSprite(), m_Rect.a, m_EmitterCountRange, m_pParticleDesc->GetDesc());
			}

			// 重置时间
			m_NextTime = m_pPool->RandFloat(m_EmitterTimeRange.x, m_EmitterTimeRange.y);
		}
	}
}

void fuiParticleEmitter::Render(fuiGraphics* pGraph)
{}
