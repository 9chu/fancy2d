#include "fuiButton.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiButton::fuiButton(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_ButtonState(BUTTONSTATE_DEFAULT),
	m_LabelOffsetWhenDown(2.f, 2.f)
{
	// 默认参数
	m_bClip = true;

	// 创建子控件
	m_pTextLabel.DirectSet(new fuiLabel(pRootPage, Name + L".TextLabel"));
	m_pTextLabel->SetParent(this);
	m_pTextLabel->SetMouseTrans(true);
	m_pTextLabel->SetClip(false);
	m_pTextLabel->RawSetProperty(L"HAlign", L"Center");
	m_pTextLabel->RawSetProperty(L"VAlign", L"Center");

	// 设置访问器
	m_DefaultSprite_Accessor = fuiPropertyAccessor<wstring>(
			&m_DefaultSprite,
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
	m_MouseOnSprite_Accessor = fuiPropertyAccessor<wstring>(
			&m_MouseOnSprite,
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
	m_MouseDownSprite_Accessor = fuiPropertyAccessor<wstring>(
			&m_MouseDownSprite,
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
	m_LabelOffsetWhenDown_Accessor = fuiPropertyAccessor<fcyVec2>(&m_LabelOffsetWhenDown);

	// 注册属性
	RegisterProperty(L"DefaultSprite", &m_DefaultSprite_Accessor);
	RegisterProperty(L"MouseOnSprite", &m_MouseOnSprite_Accessor);
	RegisterProperty(L"MouseDownSprite", &m_MouseDownSprite_Accessor);
	RegisterProperty(L"LabelOffsetWhenDown", &m_LabelOffsetWhenDown_Accessor);

	// 设置事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiButton::OnStyleChanged);
	GetEvent(L"OnSizeChanged") += fuiDelegate::EventCallBack(this, &fuiButton::OnSizeChanged);

	GetEvent(L"OnMouseMove") += fuiDelegate::EventCallBack(this, &fuiButton::OnMouseMove);
	GetEvent(L"OnMouseLeave") += fuiDelegate::EventCallBack(this, &fuiButton::OnMouseLeave);
	GetEvent(L"OnMouseLDown") += fuiDelegate::EventCallBack(this, &fuiButton::OnMouseLButtonDown);
	GetEvent(L"OnMouseLUp") += fuiDelegate::EventCallBack(this, &fuiButton::OnMouseLButtonUp);
}

fuiButton::~fuiButton()
{}

void fuiButton::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_pDefault = (fuiBorderSprite*)GetControlStyle()->QueryRes(m_DefaultSprite);
	m_pMouseOn = (fuiBorderSprite*)GetControlStyle()->QueryRes(m_MouseOnSprite);
	m_pMouseDown = (fuiBorderSprite*)GetControlStyle()->QueryRes(m_MouseDownSprite);

	if(!m_DefaultSprite.empty() && !m_pDefault)
		throw fcyException("fuiButton::OnStyleChanged", "Res not found.");
	if(!m_MouseOnSprite.empty() && !m_pMouseOn)
		throw fcyException("fuiButton::OnStyleChanged", "Res not found.");
	if(!m_MouseDownSprite.empty() && !m_pMouseDown)
		throw fcyException("fuiButton::OnStyleChanged", "Res not found.");

	if(m_pDefault && m_pDefault->GetResType() != fuiRes::RESTYPE_BORDERSPRITE)
		throw fcyException("fuiButton::OnStyleChanged", "Res type error.");
	if(m_pMouseOn && m_pMouseOn->GetResType() != fuiRes::RESTYPE_BORDERSPRITE)
		throw fcyException("fuiButton::OnStyleChanged", "Res type error.");
	if(m_pMouseDown && m_pMouseDown->GetResType() != fuiRes::RESTYPE_BORDERSPRITE)
		throw fcyException("fuiButton::OnStyleChanged", "Res type error.");
}

void fuiButton::OnSizeChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_pTextLabel->SetRect(fcyRect(0.f, 0.f, m_Rect.GetWidth(), m_Rect.GetHeight()));

	if(m_ButtonState == BUTTONSTATE_MOUSEDOWN)
	{
		m_pTextLabel->SetLeft(m_LabelOffsetWhenDown.x);
		m_pTextLabel->SetTop(m_LabelOffsetWhenDown.y);
	}
}

void fuiButton::OnMouseMove(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_ButtonState != BUTTONSTATE_MOUSEDOWN)
		m_ButtonState = BUTTONSTATE_MOUSEON;
}

void fuiButton::OnMouseLeave(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_ButtonState != BUTTONSTATE_MOUSEDOWN)
		m_ButtonState = BUTTONSTATE_DEFAULT;
}

void fuiButton::OnMouseLButtonDown(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_ButtonState != BUTTONSTATE_MOUSEDOWN)
	{
		m_ButtonState = BUTTONSTATE_MOUSEDOWN;
		m_pTextLabel->SetLeft(m_LabelOffsetWhenDown.x);
		m_pTextLabel->SetTop(m_LabelOffsetWhenDown.y);
	}
}

void fuiButton::OnMouseLButtonUp(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_ButtonState == BUTTONSTATE_MOUSEDOWN)
	{
		m_ButtonState = BUTTONSTATE_MOUSEON;
		m_pTextLabel->SetLeft(0.f);
		m_pTextLabel->SetTop(0.f);
	}
}

void fuiButton::Update(fDouble ElapsedTime)
{}

void fuiButton::Render(fuiGraphics* pGraph)
{
	fcyRect tRect(0.f, 0.f, m_Rect.GetWidth(), m_Rect.GetHeight());

	switch(m_ButtonState)
	{
	case BUTTONSTATE_DEFAULT:
		if(m_pDefault)
			m_pDefault->Draw(pGraph, tRect);
		break;
	case BUTTONSTATE_MOUSEDOWN:
		if(m_pMouseDown)
			m_pMouseDown->Draw(pGraph, tRect);
		break;
	case BUTTONSTATE_MOUSEON:
		if(m_pMouseOn)
			m_pMouseOn->Draw(pGraph, tRect);
		break;
	}

	fuiControl::Render(pGraph);
}
