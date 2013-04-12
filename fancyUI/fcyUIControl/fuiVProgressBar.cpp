#include "fuiVProgressBar.h"

#include "../fcyUIBase/fuiContainer.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiVProgressBar::ControlName[] = L"VProgressBar";

fuiVProgressBar::fuiVProgressBar(fuiContainer* pContainer, fFloat Value)
	: fuiControl(pContainer)
{
	SetValue(Value);
}

fuiVProgressBar::~fuiVProgressBar(void)
{
}

void fuiVProgressBar::recalcuRect(const fcyRect& UIRect)
{
	float tHalfXPos = UIRect.a.x + UIRect.GetWidth() / 2;

	m_Top = fcyRect(
		tHalfXPos - m_Style.Width / 2,
		UIRect.a.y,
		tHalfXPos + m_Style.Width / 2,
		UIRect.a.y + m_Style.Margin.x
		);
	m_Center = fcyRect(
		tHalfXPos - m_Style.Width / 2,
		UIRect.a.y + m_Style.Margin.x,
		tHalfXPos + m_Style.Width / 2,
		UIRect.b.y - m_Style.Margin.y
		);
	m_Bottom = fcyRect(
		tHalfXPos - m_Style.Width / 2,
		UIRect.b.y - m_Style.Margin.y,
		tHalfXPos + m_Style.Width / 2,
		UIRect.b.y
		);
	m_TopBar = fcyRect(
		tHalfXPos - m_Style.BarWidth / 2,
		UIRect.a.y,
		tHalfXPos + m_Style.BarWidth / 2,
		UIRect.a.y + m_Style.BarMargin.x
		);
	m_CenterBar = fcyRect(
		tHalfXPos - m_Style.BarWidth / 2,
		UIRect.a.y + m_Style.BarMargin.x,
		tHalfXPos + m_Style.BarWidth / 2,
		UIRect.b.y - m_Style.BarMargin.y
		);
	m_BottomBar = fcyRect(
		tHalfXPos - m_Style.BarWidth / 2,
		UIRect.b.y - m_Style.BarMargin.y,
		tHalfXPos + m_Style.BarWidth / 2,
		UIRect.b.y
		);
}

void fuiVProgressBar::OnRender(fuiRenderer* pRenderer)
{
	if(GetStyle() == NULL)
		return;

	f2dGraphics2D* pGraph = pRenderer->GetGraph();

	m_Style.Top->Draw(pGraph, m_Top);
	m_Style.Center->Draw(pGraph, m_Center);
	m_Style.Bottom->Draw(pGraph, m_Bottom);

	// 计算长度
	const fcyRect& UIRect = GetRect();
	float tHalfXPos = UIRect.a.x + UIRect.GetWidth() / 2;
	float tBarHeight = m_Value * UIRect.GetHeight();
	fcyRect tBar(tHalfXPos - m_Style.BarWidth / 2, 0.f, tHalfXPos + m_Style.BarWidth / 2, 0.f);
	fcyRect tBarUV(0.f, 0.f, 1.f, 1.f);
	if(m_Style.TopToBottom)
	{
		float tempValue = UIRect.GetHeight() - m_Style.BarMargin.y;

		// 自顶向下
		if(tBarHeight < m_Style.BarMargin.x)
		{
			tBar.a.y = UIRect.a.y;
			tBar.b.y = UIRect.a.y + tBarHeight;
			tBarUV.a.y = 0.f;
			tBarUV.b.y = tBarHeight / m_Style.BarMargin.x;

			m_Style.TopBar->Draw(pGraph, tBar, tBarUV);
		}
		else if(tBarHeight < tempValue)
		{
			tBar.a.y = UIRect.a.y + m_Style.BarMargin.x;
			tBar.b.y = UIRect.a.y + tBarHeight;
			tBarUV.a.y = 0.f;
			tBarUV.b.y = (tBarHeight - m_Style.BarMargin.x) / (tempValue - m_Style.BarMargin.x);

			m_Style.TopBar->Draw(pGraph, m_TopBar);
			m_Style.CenterBar->Draw(pGraph, tBar, tBarUV);
		}
		else
		{
			tBar.a.y = UIRect.b.y - m_Style.BarMargin.y;
			tBar.b.y = tBar.a.y + ( tBarHeight - tempValue );
			tBarUV.a.y = 0.f;
			tBarUV.b.y = ( tBarHeight - tempValue ) / m_Style.BarMargin.y;

			m_Style.TopBar->Draw(pGraph, m_TopBar);
			m_Style.CenterBar->Draw(pGraph, m_CenterBar);
			m_Style.BottomBar->Draw(pGraph, tBar, tBarUV);
		}
	}
	else
	{
		float tempValue = UIRect.GetHeight() - m_Style.BarMargin.x;

		// 自下而上
		if(tBarHeight < m_Style.BarMargin.y)
		{
			tBar.a.y = UIRect.b.y - tBarHeight;
			tBar.b.y = UIRect.b.y;
			tBarUV.a.y = 1.f - tBarHeight / m_Style.BarMargin.y;
			tBarUV.b.y = 1.f;

			m_Style.BottomBar->Draw(pGraph, tBar, tBarUV);
		}
		else if(tBarHeight < tempValue)
		{
			tBar.a.y = UIRect.b.y - tBarHeight;
			tBar.b.y = UIRect.b.y - m_Style.BarMargin.y;
			tBarUV.a.y = 1.f - (tBarHeight - m_Style.BarMargin.y) / (tempValue - m_Style.BarMargin.y);
			tBarUV.b.y = 1.f;

			m_Style.BottomBar->Draw(pGraph, m_BottomBar);
			m_Style.CenterBar->Draw(pGraph, tBar, tBarUV);
		}
		else
		{
			tBar.b.y = UIRect.a.y + m_Style.BarMargin.x;
			tBar.a.y = tBar.b.y - ( tBarHeight - tempValue );
			tBarUV.a.y = 1.f - ( tBarHeight - tempValue ) / m_Style.BarMargin.x;
			tBarUV.b.y = 1.f;

			m_Style.BottomBar->Draw(pGraph, m_BottomBar);
			m_Style.CenterBar->Draw(pGraph, m_CenterBar);
			m_Style.TopBar->Draw(pGraph, tBar, tBarUV);
		}
	}
}

void fuiVProgressBar::OnResized(fcyRect& NewSize)
{
	recalcuRect(NewSize);
}

void fuiVProgressBar::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiVProgressBar::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiVProgressBar::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiVProgressBar::OnStyleChanged", "Node 'Style' not exsited.");
	fcyXmlNode* pNode_Sprite = pStyleNode->GetNodeByName(L"Sprite", 0);
	if(!pNode_Sprite)
		throw fcyException("fuiVProgressBar::OnStyleChanged", "Node 'Sprite' not exsited.");

	// 获得参数
	fBool tFlag = true;
	VProgressBarStyle tStyle;
	tFlag |= fuiHelper::AttributeToBool(pNode_Style, L"TopToBottom", tStyle.TopToBottom);
	tFlag |= fuiHelper::AttributeToFloat(pNode_Style, L"Width", tStyle.Width);
	tFlag |= fuiHelper::AttributeToVec2(pNode_Style, L"Margin", tStyle.Margin);
	tFlag |= fuiHelper::AttributeToFloat(pNode_Style, L"BarWidth", tStyle.BarWidth);
	tFlag |= fuiHelper::AttributeToVec2(pNode_Style, L"BarMargin", tStyle.BarMargin);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Top", tStyle.Top);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Center", tStyle.Center);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Bottom", tStyle.Bottom);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarTop", tStyle.TopBar);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarCenter", tStyle.CenterBar);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarBottom", tStyle.BottomBar);
	if(tFlag == false)
		throw fcyException("fuiVProgressBar::OnStyleChanged", "Err while parse style.");

	fcyVec2 tMinSize(
		max(tStyle.BarWidth, tStyle.Width),
		max(tStyle.Margin.x + tStyle.Margin.y, tStyle.BarMargin.x + tStyle.BarMargin.y)
		);

	m_Style = tStyle;

	setMinSize(tMinSize);

	recalcuRect(GetRect());
}

void fuiVProgressBar::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out+=L",";
	Out+=ControlName;
}

fFloat fuiVProgressBar::GetValue()
{
	return m_Value;
}

void fuiVProgressBar::SetValue(fFloat Value)
{
	if(Value > 1.f)
		Value = 1.f;
	else if(Value < 0.f)
		Value = 0.f;
	m_Value = Value;
}
