#include "fuiEditBox.h"

#include "fuiExceptionMacro.h"

#include "../fcyUIBase/fuiPage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiEditBox::fuiEditBox(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_LeftTextWidth(0), m_CursorPos(0), m_CursorLeft(0),
	m_pFontProvider(NULL), m_pFontRenderer(NULL), m_bInText(false), m_OffsetToInputBox(fcyVec2(0.f, 3.f)),
	m_CursorDrawAtY(0.f), m_bCursorVisiable(false), m_CursorDisplayInterval(1.f), m_Timer(0),
	m_bFitCursor(true)
{
	// 设置访问器
	m_Text_Accessor = fuiPropertyAccessor<wstring>(
		&m_Text,
		[&](std::wstring& Prop, const std::wstring* Value)
		{
			Prop = *Value;
		},
		[&](const std::wstring& Prop, std::wstring* Value)
		{
			*Value = Prop;
			ExecEvent(L"OnTextChanged");
		}
	);
	m_FontName_Accessor = fuiPropertyAccessor<wstring>
	(
		&m_FontName,
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
	m_FontColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_FontColor);
	m_BlendColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_BlendColor);
	m_CursorSprite_Accessor = fuiPropertyAccessor<wstring>
	(
		&m_CursorSprite,
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
	m_Margin_Accessor = fuiPropertyAccessor<fcyRect>(
		&m_Margin,
		[&](std::wstring& Prop, const fcyRect* Value)
		{
			fuiPropertyAccessorHelper<fcyRect>::DefaultGetter(Prop, Value);
		},
		[&](const std::wstring& Prop, fcyRect* Value)
		{
			fuiPropertyAccessorHelper<fcyRect>::DefaultSetter(Prop, Value);

			ExecEvent(L"OnSizeChanged");
		}
	);
	m_BorderSprite_Accessor = fuiPropertyAccessor<wstring>
	(
		&m_BorderSprite,
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
	m_ActiveBorderSprite_Accessor = fuiPropertyAccessor<wstring>
	(
		&m_ActiveBorderSprite,
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
	m_OffsetToInputBox_Accessor = fuiPropertyAccessor<fcyVec2>(&m_OffsetToInputBox);
	m_CursorDisplayInterval_Accessor = fuiPropertyAccessor<float>(&m_CursorDisplayInterval);
	m_FitCursor_Accessor = fuiPropertyAccessor<fBool>(&m_bFitCursor);

	// 注册属性
	RegisterProperty(L"Text", &m_Text_Accessor);
	RegisterProperty(L"FontName", &m_FontName_Accessor);
	RegisterProperty(L"FontColor", &m_FontColor_Accessor);
	RegisterProperty(L"BlendColor", &m_BlendColor_Accessor);
	RegisterProperty(L"Margin", &m_Margin_Accessor);
	RegisterProperty(L"CursorSprite", &m_CursorSprite_Accessor);
	RegisterProperty(L"BorderSprite", &m_BorderSprite_Accessor);
	RegisterProperty(L"ActiveBorderSprite", &m_ActiveBorderSprite_Accessor);
	RegisterProperty(L"OffsetToInputBox", &m_OffsetToInputBox_Accessor);
	RegisterProperty(L"CursorDisplayInterval", &m_CursorDisplayInterval_Accessor);
	RegisterProperty(L"FitCursor", &m_FitCursor_Accessor);

	// 注册事件
	RegisterEvent(L"OnTextChanged");

	// 设置事件
	GetEvent(L"OnTextChanged") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnTextChanged);
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnStyleChanged);
	GetEvent(L"OnSizeChanged") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnSizeChanged);

	GetEvent(L"OnCharInput") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnCharInput);
	GetEvent(L"OnKeyUp") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnKeyUp);
	GetEvent(L"OnKeyDown") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnKeyDown);

	GetEvent(L"OnGetFocus") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnGetFocus);
	GetEvent(L"OnLostFocus") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnLostFocus);

	GetEvent(L"OnMouseLDown") += fuiDelegate::EventCallBack(this, &fuiEditBox::OnMouseLDown);
}

fuiEditBox::~fuiEditBox()
{
}

void fuiEditBox::adjustPos()
{
	if(m_pFontProvider && m_pFontRenderer)
	{
		float tTextLen = m_pFontRenderer->MeasureStringWidth(m_Text.substr(0, m_CursorPos).c_str());
		m_LeftTextWidth = tTextLen;

		m_CursorLeft = m_TextDrawAt.x + m_LeftTextWidth;
		if(m_CursorLeft < m_Margin.a.x)
		{
			m_CursorLeft = m_Margin.a.x;
			if(m_CursorPos)
			{
				tTextLen = m_pFontRenderer->MeasureStringWidth(m_Text.substr(m_CursorPos - 1, 1).c_str());
				m_CursorLeft += tTextLen;
			}

			m_TextDrawAt.x = m_CursorLeft - m_LeftTextWidth;
		}
		else if(m_CursorLeft > m_Rect.GetWidth() - m_Margin.b.x)
		{
			m_CursorLeft = m_Rect.GetWidth() - m_Margin.b.x;

			m_TextDrawAt.x = m_CursorLeft - m_LeftTextWidth;
		}
	}

	if(m_bInText)
	{
		fuiPositionEventArgs tPos;
		tPos.SetPos(GetAbsolutePos() + fcyVec2(m_CursorLeft, m_TextDrawAt.y) + m_OffsetToInputBox - m_pRootPage->GetRect().a);
		m_pRootPage->ExecEvent(L"OnTextInputPosChanged", &tPos);
	}
}

void fuiEditBox::OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_CursorPos > m_Text.size())
		m_CursorPos = m_Text.size();

	adjustPos();

	ExecEvent(L"OnTextChanged");
}

void fuiEditBox::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiFont> tFont;
	fcyRefPointer<fuiSprite> tCursor;
	fcyRefPointer<fuiBorderSprite> tBorder;
	fcyRefPointer<fuiBorderSprite> tActiveBorder;

	FUIGETRESANDCHECK(m_FontName, tFont, fuiFont, fuiRes::RESTYPE_FONT, "fuiEditBox::OnStyleChanged");
	FUIGETRESANDCHECK(m_CursorSprite, tCursor, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiEditBox::OnStyleChanged");
	FUIGETRESANDCHECK(m_BorderSprite, tBorder, fuiBorderSprite, fuiRes::RESTYPE_BORDERSPRITE, "fuiEditBox::OnStyleChanged");
	FUIGETRESANDCHECK(m_ActiveBorderSprite, tActiveBorder, fuiBorderSprite, fuiRes::RESTYPE_BORDERSPRITE, "fuiEditBox::OnStyleChanged");
	
	m_Font = tFont;
	m_Cursor = tCursor;
	m_Border = tBorder;
	m_ActiveBorder = tActiveBorder;

	m_pFontProvider = NULL;
	m_pFontRenderer = NULL;
	if(m_Font)
	{
		m_pFontProvider = ((fuiFont*)m_Font)->GetFontProvider();
		m_pFontRenderer = ((fuiFont*)m_Font)->GetFontRenderer();

		m_TextDrawAt.y = m_Margin.a.y + (m_Rect.GetHeight() - m_Margin.a.y - m_Margin.b.y) / 2.f - m_pFontProvider->GetLineHeight() / 2.f + m_pFontProvider->GetAscender();
		m_CursorDrawAtY = m_Margin.a.y + (m_Rect.GetHeight() - m_Margin.a.y - m_Margin.b.y) / 2.f;

		// 重新布局
		m_CursorPos = 0;
		OnTextChanged(pThis, pArgs);
		OnSizeChanged(pThis, pArgs);
	}
}

void fuiEditBox::OnSizeChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	if(m_pFontProvider && m_pFontRenderer)
	{
		float tHeight = GetHeight() - m_Margin.a.y - m_Margin.b.y;
		m_TextDrawAt.y = m_Margin.a.y + tHeight / 2.f - m_pFontProvider->GetLineHeight() / 2.f + m_pFontProvider->GetAscender();
	}
}

void fuiEditBox::OnCharInput(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiCharEventArgs* pCharArgs = (fuiCharEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_CHAR);

	switch(pCharArgs->GetChar())
	{
	case L'\b':
		if(m_CursorPos)
		{
			m_Text.erase(m_CursorPos - 1, 1);
			m_CursorPos--;
		}
		break;
	default:
		if(!iswcntrl(pCharArgs->GetChar()) || pCharArgs->GetChar() == L'\t')
		{
			m_Text.insert(m_CursorPos, 1, pCharArgs->GetChar());
			m_CursorPos++;
		}
	}
	
	OnTextChanged(pThis, pArgs);
	
	m_bCursorVisiable = true;
	m_Timer = 0.;
}

void fuiEditBox::OnKeyUp(fuiControl* pThis, fuiEventArgs* pArgs)
{
}

void fuiEditBox::OnKeyDown(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiKeyEventArgs* pKeyArgs = (fuiKeyEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_KEY);
	switch(pKeyArgs->GetKeyCode())
	{
	case F2DINPUTKEYCODE_LEFT:
		if(m_CursorPos)
		{
			m_CursorPos--;
			adjustPos();
		}
		break;
	case F2DINPUTKEYCODE_RIGHT:
		if(m_CursorPos < m_Text.size())
		{
			m_CursorPos++;
			adjustPos();
		}
		break;
	}

	m_bCursorVisiable = true;
	m_Timer = 0.;
}

void fuiEditBox::OnGetFocus(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInText = true;

	m_pRootPage->ExecEvent(L"OnTextInputStart");

	// 重新设置输入框位置
	adjustPos();

	m_bCursorVisiable = true;
	m_Timer = 0.;
}

void fuiEditBox::OnLostFocus(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInText = false;

	m_pRootPage->ExecEvent(L"OnTextInputEnd");

	m_bCursorVisiable = false;
	m_Timer = 0;
}

void fuiEditBox::OnMouseLDown(fuiControl* pThis, fuiEventArgs* pArgs)
{
	// 从左向右扫描字符串来定位光标
	if(m_pFontProvider && m_pFontRenderer)
	{
		fcyVec2 tPos = m_pRootPage->GetMouseLastPos() - GetAbsolutePos();

		float tValue = m_TextDrawAt.x;
		float tLastLen = 0.f;
		fuInt i = 0;

		for(i = 0; i<m_Text.length(); ++i)
		{
			fCharW tStr[2] = { m_Text[i] , 0 };
			float tTextLen = m_pFontRenderer->MeasureStringWidth(tStr);

			tValue += tTextLen;
			if(tPos.x <= tValue)
			{
				if(tPos.x - tLastLen <= tTextLen / 2.f)
					break;
				else
				{
					++i;
					break;
				}
			}
			else if(i == m_Text.length() - 1)
			{
				i = m_Text.length();
				break;
			}
			else
			{
				tLastLen = tValue;
			}
		}

		m_CursorPos = i;
		adjustPos();

		m_Timer = 0;
		m_bCursorVisiable = true;
	}
}

void fuiEditBox::Update(fDouble ElapsedTime)
{
	if(m_bInText)
	{
		m_Timer += ElapsedTime;
		if(m_Timer >= m_CursorDisplayInterval)
		{
			m_Timer = 0;
			m_bCursorVisiable = !m_bCursorVisiable;
		}
	}
}

void fuiEditBox::Render(fuiGraphics* pGraph)
{
	fcyRect tTextRect(m_Margin.a.x, m_Margin.a.y, GetWidth() - m_Margin.b.x, GetHeight() - m_Margin.b.y);
	fcyRect tRect(0.f, 0.f, m_Rect.GetWidth(), m_Rect.GetHeight());

	if(m_bInText)
	{
		if(m_ActiveBorder)
			m_ActiveBorder->Draw(pGraph, tRect, m_BlendColor);
	}
	else
	{
		if(m_Border)
			m_Border->Draw(pGraph, tRect, m_BlendColor);
	}

	pGraph->PushClipRect(tTextRect);
	if(m_pFontProvider && m_pFontRenderer)
	{
		m_pFontRenderer->SetColor(m_FontColor);
		m_pFontRenderer->DrawTextW(pGraph->GetGraphics(), m_Text.c_str(), m_TextDrawAt);
	}
	pGraph->PopClipRect();

	if(m_bCursorVisiable && m_Cursor)
	{
		if(m_bFitCursor && m_pFontProvider)
		{
			float tS = m_pFontProvider->GetLineHeight() / m_Cursor->GetSprite()->GetTexRect().GetHeight();
			m_Cursor->GetSprite()->SetColor(0xFF000000);
			m_Cursor->GetSprite()->Draw(pGraph->GetGraphics(), fcyVec2(m_CursorLeft, m_CursorDrawAtY), fcyVec2(1.f, tS));
		}
		else
		{
			m_Cursor->GetSprite()->SetColor(0xFF000000);
			m_Cursor->GetSprite()->Draw(pGraph->GetGraphics(), fcyVec2(m_CursorLeft, m_CursorDrawAtY));
		}
	}
}
