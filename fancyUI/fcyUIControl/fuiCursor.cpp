#include "fuiCursor.h"

#include "../fcyUIBase/fuiPage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiCursor::fuiCursor(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name)
{
	// 默认属性
	SetMouseTrans(true);
	SetLayerPriority(FUICURSOR_LAYERPRIORITY);

	m_CursorSprite_Accessor = fuiPropertyAccessor<wstring>(&m_CursorSprite);
	m_BlendColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_BlendColor);

	// 注册属性
	RegisterProperty(L"CursorSprite", &m_CursorSprite_Accessor);
	RegisterProperty(L"BlendColor", &m_BlendColor_Accessor);

	// 注册事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiCursor::OnStyleChanged);
	pRootPage->GetEvent(L"OnGlobalMouseMove") += fuiDelegate::EventCallBack(this, &fuiCursor::OnGlobalMouseMove);
}

fuiCursor::~fuiCursor()
{}

void fuiCursor::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_pCursorSprite = (fuiSprite*)GetControlStyle()->QueryRes(m_CursorSprite);

	if(!m_CursorSprite.empty() && !m_pCursorSprite)
		throw fcyException("fuiCursor::OnStyleChanged", "Res not found.");
	if(m_pCursorSprite && m_pCursorSprite->GetResType() != fuiRes::RESTYPE_SPRITE)
		throw fcyException("fuiCursor::OnStyleChanged", "Res type error.");
}

void fuiCursor::OnGlobalMouseMove(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiPositionEventArgs* pPosArgs = (fuiPositionEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_POSITION);
	
	fcyVec2 tOffset = GetAbsolutePos();
	tOffset = pPosArgs->GetPos() - tOffset;
	SetRect(fcyRect(m_Rect.a + tOffset, m_Rect.b + tOffset));
}

void fuiCursor::Update(fDouble ElapsedTime)
{
}

void fuiCursor::Render(fuiGraphics* pGraph)
{
	if(m_pCursorSprite)
	{
		f2dSprite* p = m_pCursorSprite->GetSprite();
		p->SetColor(m_BlendColor);
		p->Draw(pGraph->GetGraphics(), fcyVec2());
	}
}
