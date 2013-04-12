#include "fuiHProgressBar.h"

#include "../fcyUIBase/fuiContainer.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiHProgressBar::ControlName[] = L"HProgressBar";

fuiHProgressBar::fuiHProgressBar(fuiContainer* pContainer, fFloat Value)
	: fuiControl(pContainer)
{
	SetValue(Value);
}

fuiHProgressBar::~fuiHProgressBar(void)
{}

void fuiHProgressBar::recalcuRect(const fcyRect& UIRect)
{
	float tHalfYPos = UIRect.a.y + UIRect.GetHeight() / 2;

	m_Left = fcyRect(
		UIRect.a.x,
		tHalfYPos - m_Style.Height / 2,
		UIRect.a.x + m_Style.Margin.x,
		tHalfYPos + m_Style.Height / 2
		);
	m_Center = fcyRect(
		UIRect.a.x + m_Style.Margin.x,
		tHalfYPos - m_Style.Height / 2,
		UIRect.b.x - m_Style.Margin.y,
		tHalfYPos + m_Style.Height / 2
		);
	m_Right = fcyRect(
		UIRect.b.x - m_Style.Margin.y,
		tHalfYPos - m_Style.Height / 2,
		UIRect.b.x,
		tHalfYPos + m_Style.Height / 2
		);
	m_LeftBar = fcyRect(
		UIRect.a.x,
		tHalfYPos - m_Style.BarHeight / 2,
		UIRect.a.x + m_Style.BarMargin.x,
		tHalfYPos + m_Style.BarHeight / 2
		);
	m_CenterBar = fcyRect(
		UIRect.a.x + m_Style.BarMargin.x,
		tHalfYPos - m_Style.BarHeight / 2,
		UIRect.b.x - m_Style.BarMargin.y,
		tHalfYPos + m_Style.BarHeight / 2
		);
	m_RightBar = fcyRect(
		UIRect.b.x - m_Style.BarMargin.y,
		tHalfYPos - m_Style.BarHeight / 2,
		UIRect.b.x,
		tHalfYPos + m_Style.BarHeight / 2
		);
}

void fuiHProgressBar::OnRender(fuiRenderer* pRenderer)
{
	if(GetStyle() == NULL)
		return;

	f2dGraphics2D* pGraph = pRenderer->GetGraph();

	m_Style.Left->Draw(pGraph, m_Left);
	m_Style.Center->Draw(pGraph, m_Center);
	m_Style.Right->Draw(pGraph, m_Right);

	// 计算长度
	const fcyRect& UIRect = GetRect();
	float tHalfYPos = UIRect.a.y + UIRect.GetHeight() / 2;
	float tBarWidth = m_Value * UIRect.GetWidth();
	fcyRect tBar(0.f, tHalfYPos - m_Style.BarHeight / 2, 0.f, tHalfYPos + m_Style.BarHeight / 2);
	fcyRect tBarUV(0.f, 0.f, 1.f, 1.f);
	if(m_Style.RightToLeft)
	{
		float tempValue = UIRect.GetWidth() - m_Style.BarMargin.x;

		// 从右至左
		if(tBarWidth < m_Style.BarMargin.y)
		{
			tBar.a.x = UIRect.b.x - tBarWidth;
			tBar.b.x = UIRect.b.x;
			tBarUV.a.x = 1.f - tBarWidth / m_Style.BarMargin.y;
			tBarUV.b.x = 1.f;

			m_Style.RightBar->Draw(pGraph, tBar, tBarUV);
		}
		else if(tBarWidth < tempValue)
		{
			tBar.a.x = UIRect.b.x - tBarWidth;
			tBar.b.x = UIRect.b.x - m_Style.BarMargin.y;
			tBarUV.a.x = 1.f - (tBarWidth - m_Style.BarMargin.y) / (UIRect.GetWidth() - m_Style.BarMargin.x - m_Style.BarMargin.y);
			tBarUV.b.x = 1.f;

			m_Style.RightBar->Draw(pGraph, m_RightBar);
			m_Style.CenterBar->Draw(pGraph, tBar, tBarUV);
		}
		else
		{
			tBar.b.x = UIRect.a.x + m_Style.BarMargin.x;
			tBar.a.x = tBar.b.x - (tBarWidth - tempValue);
			tBarUV.a.x = 1.f - (tBarWidth - tempValue) / m_Style.BarMargin.x;
			tBarUV.b.x = 1.f;

			m_Style.RightBar->Draw(pGraph, m_RightBar);
			m_Style.CenterBar->Draw(pGraph, m_CenterBar);
			m_Style.LeftBar->Draw(pGraph, tBar, tBarUV);
		}
	}
	else
	{
		float tempValue = UIRect.GetWidth() - m_Style.BarMargin.y;

		// 从左至右
		if(tBarWidth < m_Style.BarMargin.x)
		{
			tBar.a.x = UIRect.a.x;
			tBar.b.x = UIRect.a.x + tBarWidth;
			tBarUV.a.x = 0.f;
			tBarUV.b.x = tBarWidth / m_Style.BarMargin.x;

			m_Style.LeftBar->Draw(pGraph, tBar, tBarUV);
		}
		else if(tBarWidth < tempValue)
		{
			tBar.a.x = UIRect.a.x + m_Style.BarMargin.x;
			tBar.b.x = UIRect.a.x + tBarWidth;
			tBarUV.a.x = 0.f;
			tBarUV.b.x = (tBarWidth - m_Style.BarMargin.x) / (UIRect.GetWidth() - m_Style.BarMargin.x - m_Style.BarMargin.y);

			m_Style.LeftBar->Draw(pGraph, m_LeftBar);
			m_Style.CenterBar->Draw(pGraph, tBar, tBarUV);
		}
		else
		{
			tBar.a.x = UIRect.b.x - m_Style.BarMargin.y;
			tBar.b.x = tBar.a.x + (tBarWidth - tempValue);
			tBarUV.a.x = 0.f;
			tBarUV.b.x = (tBarWidth - tempValue) / m_Style.BarMargin.y;

			m_Style.LeftBar->Draw(pGraph, m_LeftBar);
			m_Style.CenterBar->Draw(pGraph, m_CenterBar);
			m_Style.RightBar->Draw(pGraph, tBar, tBarUV);
		}
	}
}

void fuiHProgressBar::OnResized(fcyRect& NewSize)
{
	recalcuRect(NewSize);
}

void fuiHProgressBar::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiHProgressBar::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiHProgressBar::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiHProgressBar::OnStyleChanged", "Node 'Style' not exsited.");
	fcyXmlNode* pNode_Sprite = pStyleNode->GetNodeByName(L"Sprite", 0);
	if(!pNode_Sprite)
		throw fcyException("fuiHProgressBar::OnStyleChanged", "Node 'Sprite' not exsited.");

	// 获得参数
	fBool tFlag = true;
	HProgressBarStyle tStyle;
	tFlag |= fuiHelper::AttributeToBool(pNode_Style, L"RightToLeft", tStyle.RightToLeft);
	tFlag |= fuiHelper::AttributeToFloat(pNode_Style, L"Height", tStyle.Height);
	tFlag |= fuiHelper::AttributeToVec2(pNode_Style, L"Margin", tStyle.Margin);
	tFlag |= fuiHelper::AttributeToFloat(pNode_Style, L"BarHeight", tStyle.BarHeight);
	tFlag |= fuiHelper::AttributeToVec2(pNode_Style, L"BarMargin", tStyle.BarMargin);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Left", tStyle.Left);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Center", tStyle.Center);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Right", tStyle.Right);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarLeft", tStyle.LeftBar);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarCenter", tStyle.CenterBar);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"BarRight", tStyle.RightBar);
	if(tFlag == false)
		throw fcyException("fuiHProgressBar::OnStyleChanged", "Err while parse style.");

	fcyVec2 tMinSize(
		max(tStyle.Margin.x + tStyle.Margin.y, tStyle.BarMargin.x + tStyle.BarMargin.y),
		max(tStyle.BarHeight, tStyle.Height)
		);

	m_Style = tStyle;

	setMinSize(tMinSize);

	recalcuRect(GetRect());
}

void fuiHProgressBar::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out+=L",";
	Out+=ControlName;
}

fFloat fuiHProgressBar::GetValue()
{
	return m_Value;
}

void fuiHProgressBar::SetValue(fFloat Value)
{
	if(Value > 1.f)
		Value = 1.f;
	else if(Value < 0.f)
		Value = 0.f;
	m_Value = Value;
}
