#include "fuiImagedButton.h"

#include "fuiExceptionMacro.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiImagedButton::fuiImagedButton(fuiPage* pRootPage, const std::wstring& Name)
	: fuiImageBox(pRootPage, Name), m_bIsMoveIn(false), m_bIsMouseDown(false)
{
	m_ActiveImage_Accessor = fuiPropertyAccessor<wstring>(
		&m_ActiveImage,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_ActiveImage = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_ClickImage_Accessor = fuiPropertyAccessor<wstring>(
		&m_ClickImage,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_ClickImage = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_MouseEventMargin_Accessor = fuiPropertyAccessor<fcyRect>(&m_MouseEventMargin);

	// 注册属性
	RegisterProperty(L"ActiveImage", &m_ActiveImage_Accessor);
	RegisterProperty(L"ClickImage", &m_ClickImage_Accessor);
	RegisterProperty(L"MouseEventMargin", &m_MouseEventMargin_Accessor);

	// 注册事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiImagedButton::OnStyleChanged);

	GetEvent(L"OnMouseMove") += fuiDelegate::EventCallBack(this, &fuiImagedButton::OnMouseMove);
	GetEvent(L"OnMouseLeave") += fuiDelegate::EventCallBack(this, &fuiImagedButton::OnMouseLeave);
	GetEvent(L"OnMouseLDown") += fuiDelegate::EventCallBack(this, &fuiImagedButton::OnMouseLButtonDown);
	GetEvent(L"OnMouseLUp") += fuiDelegate::EventCallBack(this, &fuiImagedButton::OnMouseLButtonUp);

	// 注册事件
	RegisterEvent(L"OnClick");
}

fuiImagedButton::~fuiImagedButton()
{}

void fuiImagedButton::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiSprite> tActiveImageSprite;
	fcyRefPointer<fuiSprite> tClickImageSprite;

	FUIGETRESANDCHECK(m_ActiveImage, tActiveImageSprite, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiImagedButton::OnStyleChanged");
	FUIGETRESANDCHECK(m_ClickImage, tClickImageSprite, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiImagedButton::OnStyleChanged");

	m_pActiveImageSprite = tActiveImageSprite;
	m_pClickImageSprite = tClickImageSprite;
}

void fuiImagedButton::OnMouseMove(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bIsMoveIn = true;
}

void fuiImagedButton::OnMouseLeave(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bIsMoveIn = false;
}

void fuiImagedButton::OnMouseLButtonDown(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bIsMouseDown = true;
}

void fuiImagedButton::OnMouseLButtonUp(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_bIsMoveIn)
		ExecEvent(L"OnClick");
	
	m_bIsMouseDown = false;
}

void fuiImagedButton::Render(fuiGraphics* pGraph)
{
	// 依据状态绘制相应图片
	fuiSprite* tSprite = NULL;
	if(m_bIsMouseDown)
		tSprite = m_pClickImageSprite;
	else
	{
		if(m_bIsMoveIn)
			tSprite = m_pActiveImageSprite;
		else
			tSprite = m_pImageSprite;
	}

	if(tSprite == NULL)
		tSprite = m_pImageSprite;

	// 调用基类进行绘制
	fuiImageBox::DrawImage(pGraph, tSprite, m_BlendColor, m_ImageFillMethod);

	// 跳过fuiImageBox的Render
	fuiControl::Render(pGraph);
}

fBool fuiImagedButton::HitTest(const fcyVec2& Pos)
{
	fcyRect tEnableRange(m_MouseEventMargin.a, fcyVec2(GetWidth(), GetHeight()) - m_MouseEventMargin.b);

	return tEnableRange.Contain(Pos);
}
