#include "fuiHSliderBar.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiHSliderBarButton::ControlName[] = L"HSliderBarButton";

fuiHSliderBarButton::fuiHSliderBarButton(fuiContainer* pContainer)
	: fuiSpriteButton(pContainer), m_ButtonPos(0.f), m_bMyMouseDown(false)
{}

fuiHSliderBarButton::~fuiHSliderBarButton()
{}

fFloat fuiHSliderBarButton::calcuMaxLen(const fcyRect& ParentUISize)
{
	fuiHSliderBar::HSliderBarStyle* tStyle = &((fuiHSliderBar*)GetParent())->m_Style;
	float tMouseMaxLen = ParentUISize.GetWidth() - tStyle->SpriteButtonMargin.x - tStyle->SpriteButtonMargin.y - GetMinSize().x;
	return tMouseMaxLen;
}

void fuiHSliderBarButton::UpdatePos(const fcyRect& ParentNewSize)
{
	fuiHSliderBar::HSliderBarStyle* tStyle = &((fuiHSliderBar*)GetParent())->m_Style;

	// 相对于Margin.x的位移
	float tPos = m_ButtonPos * calcuMaxLen(ParentNewSize);

	// 位置矩形
	fcyRect tRect(
		tStyle->SpriteButtonMargin.x + tPos,
		0.f,
		tStyle->SpriteButtonMargin.x + tPos + GetMinSize().x,
		ParentNewSize.GetHeight()
		);

	// 设置位置
	SetRect(tRect);
}

fFloat fuiHSliderBarButton::GetButtonPos()
{
	return m_ButtonPos;
}

void fuiHSliderBarButton::SetButtonPos(fFloat Pos, fBool ExecEvent)
{
	m_ButtonPos = Pos;
	if(m_ButtonPos < 0.f)
		m_ButtonPos = 0.f;
	else if(m_ButtonPos > 1.f)
		m_ButtonPos = 1.f;

	if(ExecEvent && m_Evt_OnValueChanged)
		m_Evt_OnValueChanged(Pos);

	UpdatePos(GetParent()->GetRect());
}

void fuiHSliderBarButton::SetEvt_OnValueChanged(const function<void(fFloat)>& Evt)
{
	m_Evt_OnValueChanged = Evt;
}

void fuiHSliderBarButton::SetEvt_OnStartDrag(const function<void(void)>& Evt)
{
	m_Evt_OnStartDrag = Evt;
}

void fuiHSliderBarButton::SetEvt_OnEndDrag(const function<void(void)>& Evt)
{
	m_Evt_OnEndDrag = Evt;
}

fBool fuiHSliderBarButton::OnMouseMove(fFloat X, fFloat Y)
{
	if(!m_bMyMouseDown && !fuiSpriteButton::OnMouseMove(X,Y))
		return false;

	if(m_bMyMouseDown)
	{
		float tMouseMaxLen = calcuMaxLen(GetParent()->GetRect());

		X += GetRect().a.x;
		if(X < m_MouseEnable.x)
			X = m_MouseEnable.x;
		else if(X > m_MouseEnable.y)
			X = m_MouseEnable.y;

		m_ButtonPos = (X - m_MouseEnable.x) / tMouseMaxLen;

		if(m_Evt_OnValueChanged)
			m_Evt_OnValueChanged(m_ButtonPos);

		UpdatePos(GetParent()->GetRect());
	}

	return true;
}

void fuiHSliderBarButton::OnLMouseUp(fFloat X, fFloat Y)
{
	fuiSpriteButton::OnLMouseUp(X,Y);

	m_bMyMouseDown = false;

	if(m_Evt_OnEndDrag)
		m_Evt_OnEndDrag();
}

fBool fuiHSliderBarButton::OnLMouseDown(fFloat X, fFloat Y)
{
	if(!fuiSpriteButton::OnLMouseDown(X,Y))
		return false;

	m_bMyMouseDown = true;

	if(m_Evt_OnStartDrag)
		m_Evt_OnStartDrag();

	// 计算鼠标有效X范围
	fuiHSliderBar::HSliderBarStyle* tStyle = &((fuiHSliderBar*)GetParent())->m_Style;
	float tMouseMaxLen = calcuMaxLen(GetParent()->GetRect());
	m_MouseEnable.x = tStyle->SpriteButtonMargin.x + X;
	m_MouseEnable.y = m_MouseEnable.x + tMouseMaxLen;

	return true;
}

void fuiHSliderBarButton::OnQueryControlName(wstring& Out)
{
	fuiSpriteButton::OnQueryControlName(Out);

	Out += L",";
	Out += ControlName;
}

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiHSliderBar::ControlName[] = L"HSliderBar";

fuiHSliderBar::fuiHSliderBar(fuiContainer* pParent)
	: fuiContainer(pParent)
{
	m_pProgressBar = new fuiHProgressBar(this);
	m_pSpriteButton = new fuiHSliderBarButton(this);
}

fuiHSliderBar::~fuiHSliderBar(void)
{}

void fuiHSliderBar::recalcuRect(const fcyRect& UISize)
{
	m_pProgressBar->SetRect(fcyRect(
		m_Style.ProgressBarMargin.x,
		0.f, 
		UISize.GetWidth() - m_Style.ProgressBarMargin.y, 
		UISize.GetHeight())
		);
	m_pSpriteButton->UpdatePos(UISize);
}

fFloat fuiHSliderBar::GetProgPos()
{
	return m_pProgressBar->GetValue();
}

void fuiHSliderBar::SetProgPos(fFloat Pos)
{
	m_pProgressBar->SetValue(Pos);
}

fFloat fuiHSliderBar::GetButtonPos()
{
	return m_pSpriteButton->GetButtonPos();
}

void fuiHSliderBar::SetButtonPos(fFloat Pos, fBool ExecEvent)
{
	m_pSpriteButton->SetButtonPos(Pos, ExecEvent);
}

void fuiHSliderBar::SetEvt_OnValueChanged(const std::function<void(fFloat)>& Evt)
{
	m_pSpriteButton->SetEvt_OnValueChanged(Evt);
}

void fuiHSliderBar::SetEvt_OnStartDrag(const function<void(void)>& Evt)
{
	m_pSpriteButton->SetEvt_OnStartDrag(Evt);
}

void fuiHSliderBar::SetEvt_OnEndDrag(const function<void(void)>& Evt)
{
	m_pSpriteButton->SetEvt_OnEndDrag(Evt);
}

void fuiHSliderBar::OnResized(fcyRect& NewSize)
{
	if(GetStyle())
		recalcuRect(NewSize);
}

void fuiHSliderBar::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiHSliderBar::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiHSliderBar::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_SpriteButton = pStyleNode->GetNodeByName(L"SpriteButton", 0);
	if(!pNode_SpriteButton)
		throw fcyException("fuiHSliderBar::OnStyleChanged", "Node 'SpriteButton' not exsited.");
	fcyXmlNode* pNode_HProgressBar = pStyleNode->GetNodeByName(L"HProgressBar", 0);
	if(!pNode_HProgressBar)
		throw fcyException("fuiHSliderBar::OnStyleChanged", "Node 'HProgressBar' not exsited.");

	// 获得参数
	fBool tFlag = true;
	HSliderBarStyle tStyle;
	tFlag |= fuiHelper::AttributeToVec2(pNode_SpriteButton, L"Margin", tStyle.SpriteButtonMargin);
	tFlag |= fuiHelper::AttributeToVec2(pNode_HProgressBar, L"Margin", tStyle.ProgressBarMargin);
	if(tFlag == false)
		throw fcyException("fuiHSliderBar::OnStyleChanged", "Err while parse style.");
	
	// 设置样式
	m_pSpriteButton->SetStyle(pNode_SpriteButton->GetAttribute(L"Style"));
	m_pProgressBar->SetStyle(pNode_HProgressBar->GetAttribute(L"Style"));

	// 计算最小大小
	fcyVec2 tSpriteMinSize(
		tStyle.SpriteButtonMargin.x + tStyle.SpriteButtonMargin.y + m_pSpriteButton->GetMinSize().x,
		m_pSpriteButton->GetMinSize().y
		);
	fcyVec2 tProgressBarMinSize(
		tStyle.ProgressBarMargin.x + tStyle.ProgressBarMargin.y + m_pProgressBar->GetMinSize().x,
		m_pProgressBar->GetMinSize().y
		);

	fcyVec2 tMinSize(
		max(tSpriteMinSize.x, tProgressBarMinSize.x),
		max(tSpriteMinSize.y, tProgressBarMinSize.y)
		);

	m_Style = tStyle;

	setMinSize(tMinSize);

	recalcuRect(GetRect());
}

void fuiHSliderBar::OnQueryControlName(wstring& Out)
{
	fuiContainer::OnQueryControlName(Out);

	Out += L",";

	Out += ControlName;
}

fBool fuiHSliderBar::OnMouseMove(fFloat X, fFloat Y)
{
	return fuiContainer::OnMouseMove(X,Y);
}

fBool fuiHSliderBar::OnLMouseDown(fFloat X, fFloat Y)
{
	return fuiContainer::OnLMouseDown(X,Y);
}
