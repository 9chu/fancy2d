#include "fuiLabel.h"

#include <fcyMisc/fcyStringHelper.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiLabel::fuiLabel(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_HAlign(HALIGNMODE_LEFT), m_VAlign(VALIGNMODE_TOP), m_TextHeight(0),
	m_pFontProvider(NULL), m_pFontRenderer(NULL)
{
	// 默认参数
	m_bClip = true;

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
	m_HAlign_Accessor = fuiPropertyAccessor<HALIGNMODE>(
		&m_HAlign,
		[&](std::wstring& Prop, const HALIGNMODE* Value){
			switch(*Value)
			{
			case HALIGNMODE_LEFT:
				Prop = L"Left";
				break;
			case HALIGNMODE_CENTER:
				Prop = L"Center";
				break;
			case HALIGNMODE_RIGHT:
				Prop = L"Right";
				break;
			default:
				throw fcyException("lambda [&](std::wstring&, HALIGNMODE*)", "Value of HALIGNMODE is invalid.");
			}
		},
		[&](const std::wstring& Prop, HALIGNMODE* Value){
			if(_wcsicmp(Prop.c_str(), L"left")==0)
				*Value = HALIGNMODE_LEFT;
			else if(_wcsicmp(Prop.c_str(), L"center")==0)
				*Value = HALIGNMODE_CENTER;
			else if(_wcsicmp(Prop.c_str(), L"right")==0)
				*Value = HALIGNMODE_RIGHT;
			else
				throw fcyException("lambda [&](const std::wstring&, HALIGNMODE*)", "Property string is not correct.");
		}
	);
	m_VAlign_Accessor = fuiPropertyAccessor<VALIGNMODE>(
		&m_VAlign,
		[&](std::wstring& Prop, const VALIGNMODE* Value){
			switch(*Value)
			{
			case VALIGNMODE_TOP:
				Prop = L"Top";
				break;
			case VALIGNMODE_CENTER:
				Prop = L"Center";
				break;
			case VALIGNMODE_BOTTOM:
				Prop = L"Bottom";
				break;
			default:
				throw fcyException("lambda [&](std::wstring&, VALIGNMODE*)", "Value of VALIGNMODE is invalid.");
			}
		},
		[&](const std::wstring& Prop, VALIGNMODE* Value){
		if(_wcsicmp(Prop.c_str(), L"top")==0)
				*Value = VALIGNMODE_TOP;
			else if(_wcsicmp(Prop.c_str(), L"center")==0)
				*Value = VALIGNMODE_CENTER;
			else if(_wcsicmp(Prop.c_str(), L"bottom")==0)
				*Value = VALIGNMODE_BOTTOM;
			else
				throw fcyException("lambda [&](const std::wstring&, VALIGNMODE*)", "Property string is not correct.");
		}
	);

	// 注册属性
	RegisterProperty(L"Text", &m_Text_Accessor);
	RegisterProperty(L"FontName", &m_FontName_Accessor);
	RegisterProperty(L"FontColor", &m_FontColor_Accessor);
	RegisterProperty(L"HAlign", &m_HAlign_Accessor);
	RegisterProperty(L"VAlign", &m_VAlign_Accessor);

	// 注册事件
	RegisterEvent(L"OnTextChanged");

	// 设置事件
	GetEvent(L"OnTextChanged") += fuiDelegate::EventCallBack(this, &fuiLabel::OnTextChanged);
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiLabel::OnStyleChanged);
}

fuiLabel::~fuiLabel()
{}

void fuiLabel::OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_Lines.clear();
	m_LineWidth.resize(fcyStringHelper::StringSplit(m_Text, L"\n", false, m_Lines));

	// 度量每行的宽度
	if(m_pFontRenderer && m_pFontProvider)
	{
		for(fuInt i = 0; i<m_Lines.size(); ++i)
		{
			m_LineWidth[i] = m_pFontRenderer->MeasureStringWidth(m_Lines[i].c_str()) + 2.f;
		}

		m_TextHeight = m_Lines.size() * m_pFontProvider->GetLineHeight();
	}
}

void fuiLabel::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_pFontProvider = NULL;
	m_pFontRenderer = NULL;

	m_Font = (fuiFont*)GetControlStyle()->QueryRes(m_FontName);

	if(!m_FontName.empty() && !m_Font)
		throw fcyException("fuiLabel::OnStyleChanged", "Res not found.");
	if(m_Font && m_Font->GetResType() != fuiRes::RESTYPE_FONT)
		throw fcyException("fuiLabel::OnStyleChanged", "Font res error.");

	m_pFontProvider = ((fuiFont*)m_Font)->GetFontProvider();
	m_pFontRenderer = ((fuiFont*)m_Font)->GetFontRenderer();

	// 重新布局
	OnTextChanged(pThis, pArgs);
}

void fuiLabel::Update(fDouble ElapsedTime)
{}

void fuiLabel::Render(fuiGraphics* pGraph)
{
	if(m_pFontProvider && m_pFontRenderer)
	{
		// 设置颜色
		m_pFontRenderer->SetColor(m_FontColor);

		// 一行行绘制
		float tAscender = m_pFontProvider->GetAscender();
		float tDescender = m_pFontProvider->GetDescender();
		float tLineHeight = m_pFontProvider->GetLineHeight();
		for(fuInt i = 0; i<m_Lines.size(); ++i)
		{
			fcyVec2 tPos;

			// 横向位置
			switch(m_HAlign)
			{
			case HALIGNMODE_LEFT:
				tPos.x = 0.f;
				break;
			case HALIGNMODE_CENTER:
				tPos.x = GetWidth() / 2.f - m_LineWidth[i] / 2.f;
				break;
			case HALIGNMODE_RIGHT:
				tPos.x = GetWidth() - m_LineWidth[i];
				break;
			}

			// 纵向位置
			switch(m_VAlign)
			{
			case VALIGNMODE_TOP:
				tPos.y = tAscender + tLineHeight * i;
				break;
			case VALIGNMODE_CENTER:
				tPos.y = GetHeight() / 2.f - m_TextHeight / 2.f + tAscender;
				break;
			case VALIGNMODE_BOTTOM:
				tPos.y = GetHeight() + tDescender - tLineHeight * i;
				break;
			}

			m_pFontRenderer->DrawTextW(pGraph->GetGraphics(), m_Lines[i].c_str(), tPos);
		}
	}
}
