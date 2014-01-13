#include "fuiParticlePool.h"

#include "fuiExceptionMacro.h"

#include "../fcyUIBase/fuiPage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiParticlePool::fuiParticlePool(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name)
{
	// 默认参数
	m_bClip = true;
	m_bMouseTrans = true;

	m_ParticlePool_Accessor = fuiPropertyAccessor<void*>(
		(void**)&m_pParticlePool,
		fuiPropertyAccessorHelper<void*>::DefaultGetter, // 可读
		fuiPropertyAccessor<void*>::PropSetter()         // 只读
		);
	RegisterProperty(L"ParticlePoolPtr", &m_ParticlePool_Accessor);

	// 创建粒子池
	if(FCYFAILED(GetRoot()->GetRenderer()->CreateParticlePool(&m_pParticlePool)))
		throw fcyException("fuiParticlePool::fuiParticlePool", "f2dRenderer::CreateParticlePool failed.");
}

fuiParticlePool::~fuiParticlePool()
{
}

void fuiParticlePool::Update(fDouble ElapsedTime)
{
	m_pParticlePool->Update((float)ElapsedTime);

	fuiControl::Update(ElapsedTime);
}

void fuiParticlePool::Render(fuiGraphics* pGraph)
{
	m_pParticlePool->Render(pGraph->GetGraphics());

	fuiControl::Render(pGraph);
}
