#include "fuiSpriteButton.h"

#include "../fcyUIBase/fuiContainer.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiSpriteButton::ControlName[] = L"SpriteButton";

fuiSpriteButton::fuiSpriteButton(fuiContainer* pContainer)
	: fuiControl(pContainer), m_MouseState(SPRITEBUTTONSTATE_DEFAULT), m_BlendColor(0xFF000000), m_bIn(false)
{}

fuiSpriteButton::~fuiSpriteButton(void)
{}

void fuiSpriteButton::recalcuRect(const fcyRect& UIRect)
{
	fcyVec2 tCenter = UIRect.GetCenter();
	m_SpriteRect.a = tCenter - fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2);
	m_SpriteRect.b = tCenter + fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2);
}

void fuiSpriteButton::OnRender(fuiRenderer* pRenderer)
{
	if(!GetStyle())
		return;

	f2dGraphics2D* pGraph = pRenderer->GetGraph();

	switch(m_MouseState)
	{
	case SPRITEBUTTONSTATE_DEFAULT:
		m_Style.Default->SetColor(m_BlendColor);
		m_Style.Default->Draw(pGraph, m_SpriteRect);
		break;
	case SPRITEBUTTONSTATE_MOUSEON:
		m_Style.MouseOn->SetColor(m_BlendColor);
		m_Style.MouseOn->Draw(pGraph, m_SpriteRect);
		break;
	case SPRITEBUTTONSTATE_MOUSEDOWN:
		m_Style.MouseDown->SetColor(m_BlendColor);
		m_Style.MouseDown->Draw(pGraph, m_SpriteRect);
		break;
	}
}

void fuiSpriteButton::OnMouseLeave()
{
	m_MouseState = SPRITEBUTTONSTATE_DEFAULT;
	if(m_bIn && m_Evt_OnMouseOut)
		m_Evt_OnMouseOut();
	m_bIn = false;
}

fBool fuiSpriteButton::OnMouseMove(fFloat X, fFloat Y)
{
	fcyVec2 tCenter(GetRect().GetWidth() / 2, GetRect().GetHeight() / 2);
	fcyRect tRange(tCenter - fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2),
		tCenter + fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2));

	if(tRange.Contain(fcyVec2(X,Y)))
	{
		if(!m_bIn)
		{
			m_bIn = true;
			if(m_Evt_OnMouseIn)
				m_Evt_OnMouseIn();
		}

		if(m_MouseState != SPRITEBUTTONSTATE_MOUSEDOWN)
			m_MouseState = SPRITEBUTTONSTATE_MOUSEON;
		return true;
	}
	else
	{
		if(m_bIn)
		{
			m_bIn = false;
			if(m_Evt_OnMouseOut)
				m_Evt_OnMouseOut();
		}
		return false;
	}
}

void fuiSpriteButton::OnLMouseUp(fFloat X, fFloat Y)
{
	m_MouseState = SPRITEBUTTONSTATE_MOUSEON;

	fcyVec2 tCenter(GetRect().GetWidth() / 2, GetRect().GetHeight() / 2);
	fcyRect tRange(tCenter - fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2),
		tCenter + fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2));

	if(tRange.Contain(fcyVec2(X,Y)))
	{
		if(m_Evt_OnClick != nullptr)
			m_Evt_OnClick();
	}
}

fBool fuiSpriteButton::OnLMouseDown(fFloat X, fFloat Y)
{
	fcyVec2 tCenter(GetRect().GetWidth() / 2, GetRect().GetHeight() / 2);
	fcyRect tRange(tCenter - fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2),
		tCenter + fcyVec2(m_Style.Size.x / 2, m_Style.Size.y / 2));

	if(tRange.Contain(fcyVec2(X,Y)))
	{
		m_MouseState = SPRITEBUTTONSTATE_MOUSEDOWN;

		GetFocus();

		return true;
	}
	else
		return false;
}

void fuiSpriteButton::OnResized(fcyRect& NewSize)
{
	recalcuRect(NewSize);
}

void fuiSpriteButton::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiSpriteButton::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiSpriteButton::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiSpriteButton::OnStyleChanged", "Node 'Style' not exsited.");
	fcyXmlNode* pNode_Sprite = pStyleNode->GetNodeByName(L"Sprite", 0);
	if(!pNode_Sprite)
		throw fcyException("fuiSpriteButton::OnStyleChanged", "Node 'Sprite' not exsited.");

	// 获得参数
	fBool tFlag = true;
	SpriteButtonStyle tStyle;
	tFlag |= fuiHelper::AttributeToVec2(pNode_Style, L"Size", tStyle.Size);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Default", tStyle.Default);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"MouseOn", tStyle.MouseOn);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"MouseDown", tStyle.MouseDown);
	if(tFlag == false)
		throw fcyException("fuiSpriteButton::OnStyleChanged", "Err while parse style.");

	fcyVec2 tMinSize = tStyle.Size;

	m_Style = tStyle;

	setMinSize(tMinSize);

	recalcuRect(GetRect());
}

void fuiSpriteButton::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out+=L",";

	Out+=ControlName;
}

fcyColor fuiSpriteButton::GetBlendColor()
{
	return m_BlendColor;
}

void fuiSpriteButton::SetBlendColor(const fcyColor& Color)
{
	m_BlendColor = Color;
}

void fuiSpriteButton::SetEvt_OnClick(const function<void(void)>& Func)
{
	m_Evt_OnClick = Func;
}

void fuiSpriteButton::SetEvt_OnMouseIn(const function<void(void)>& Func)
{
	m_Evt_OnMouseIn = Func;
}

void fuiSpriteButton::SetEvt_OnMouseOut(const function<void(void)>& Func)
{
	m_Evt_OnMouseOut = Func;
}
