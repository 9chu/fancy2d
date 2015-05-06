#include "fuiGraphics.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiGraphics::fuiGraphics(f2dRenderer* pRenderer, f2dGraphics2D* pGraph)
	: m_pDev(pRenderer->GetDevice()), m_pGraphics2D(pGraph), m_InRender(false)
{
	if(!pRenderer)
		throw fcyException("fuiGraphics::fuiGraphics", "Param 'pRenderer' is null.");
	if(!pGraph)
		throw fcyException("fuiGraphics::fuiGraphics", "Param 'pGraph' is null.");

	if(FCYFAILED(pRenderer->CreateGeometryRenderer(&m_pGeometry)))
		throw fcyException("fuiGraphics::fuiGraphics", "f2dRenderer::CreateGeometryRenderer failed.");
}

fuiGraphics::~fuiGraphics()
{
}

void fuiGraphics::PushClipRect(const fcyRect& Rect)
{
	if(!m_InRender)
		return;

	const fcyRect& tTop = m_ClipRect.top();
	fcyRect tRect = Rect;
	tRect.a += fcyVec2(m_TransMat.m._41, m_TransMat.m._42);
	tRect.b += fcyVec2(m_TransMat.m._41, m_TransMat.m._42);

	if(!tTop.Intersect(tRect, &tRect))
		tRect = fcyRect();

	m_ClipRect.push(tRect);
	m_pGraphics2D->Flush();
	m_pDev->SetScissorRect(tRect);
}

void fuiGraphics::PopClipRect()
{
	if(!m_InRender)
		return;

	m_ClipRect.pop();

	m_pGraphics2D->Flush();
	m_pDev->SetScissorRect(m_ClipRect.top());
}

void fuiGraphics::PushOffset(const fcyVec2& Offset)
{
	if(!m_InRender)
		return;

	m_Offsets.push(Offset);

	m_TransMat.m._41 += Offset.x;
	m_TransMat.m._42 += Offset.y;

	m_pGraphics2D->SetWorldTransform(m_TransMat);
}

void fuiGraphics::PopOffset()
{
	if(!m_InRender)
		return;

	m_TransMat.m._41 -= m_Offsets.top().x;
	m_TransMat.m._42 -= m_Offsets.top().y;

	m_Offsets.pop();
	
	m_pGraphics2D->SetWorldTransform(m_TransMat);
}

fcyVec2 fuiGraphics::GetTopOffset()
{
	return m_Offsets.top();
}

void fuiGraphics::DisableOffset()
{
	if(!m_InRender)
		return;

	m_pGraphics2D->SetWorldTransform(m_OrgTransMat);
}

void fuiGraphics::RestoreOffset()
{
	if(!m_InRender)
		return;

	m_pGraphics2D->SetWorldTransform(m_TransMat);
}

fResult fuiGraphics::Begin()
{
	fResult tRet;
	if(FCYFAILED(tRet = m_pGraphics2D->Begin()))
		return tRet;

	// ÍÆÈë²Ã¼ô¾ØÐÎ
	m_ClipRect.push(m_pDev->GetScissorRect());

	// ±£´æ×´Ì¬
	m_OrgTransMat = m_pGraphics2D->GetWorldTransform();
	m_TransMat = fcyMatrix4::GetIdentity();

	m_Offsets.push(fcyVec2());

	m_InRender = true;

	return FCYERR_OK;
}

void fuiGraphics::End()
{
	m_InRender = false;
	m_pGraphics2D->End();

	// »Ø¸´×´Ì¬
	while(!m_ClipRect.empty())
	{
		m_ClipRect.pop();

		if(!m_ClipRect.empty())
			m_pDev->SetScissorRect(m_ClipRect.top());
	}
	while(!m_Offsets.empty())
		m_Offsets.pop();

	m_pGraphics2D->SetWorldTransform(m_OrgTransMat);
}
