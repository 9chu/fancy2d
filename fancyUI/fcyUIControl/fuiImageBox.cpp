#include "fuiImageBox.h"

#include "fuiExceptionMacro.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiImageBox::fuiImageBox(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_ImageFillMethod(IMAGEFILLMETHOD_LEFTTOP), 
	m_ColorBlendMethod(COLORBLENDMETHOD_DEFAULT)
{
	// 默认属性
	SetClip(false);

	m_Image_Accessor = fuiPropertyAccessor<wstring>(
		&m_Image,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_Image = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_BlendColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_BlendColor);
	m_ImageFillMethod_Accessor = fuiPropertyAccessor<IMAGEFILLMETHOD>(
		&m_ImageFillMethod,
		[&](std::wstring& Prop, const IMAGEFILLMETHOD* Value){
			switch(*Value)
			{
			case IMAGEFILLMETHOD_LEFTTOP:
				Prop = L"LeftTop";
				break;
			case IMAGEFILLMETHOD_FIT:
				Prop = L"Fit";
				break;
			case IMAGEFILLMETHOD_STRETCH:
				Prop = L"Stretch";
				break;
			default:
				throw fcyException("lambda [&](std::wstring&, const IMAGEFILLMETHOD*)", "Value of IMAGEFILLMETHOD is invalid.");
			}
		},
		[&](const std::wstring& Prop, IMAGEFILLMETHOD* Value){
			if(_wcsicmp(Prop.c_str(), L"lefttop")==0)
				*Value = IMAGEFILLMETHOD_LEFTTOP;
			else if(_wcsicmp(Prop.c_str(), L"fit")==0)
				*Value = IMAGEFILLMETHOD_FIT;
			else if(_wcsicmp(Prop.c_str(), L"stretch")==0)
				*Value = IMAGEFILLMETHOD_STRETCH;
			else
				throw fcyException("lambda [&](const std::wstring&, IMAGEFILLMETHOD*)", "Property string is not correct.");
		}
	);
	m_ColorBlendMethod_Accessor = fuiPropertyAccessor<COLORBLENDMETHOD>(
		&m_ColorBlendMethod,
		[&](std::wstring& Prop, const COLORBLENDMETHOD* Value){
			switch(*Value)
			{
			case COLORBLENDMETHOD_DEFAULT:
				Prop = L"Default";
				break;
			case COLORBLENDMETHOD_ADD:
				Prop = L"Add";
				break;
			case COLORBLENDMETHOD_SUBTRACT:
				Prop = L"Subtract";
				break;
			case COLORBLENDMETHOD_MODULATE:
				Prop = L"Modulate";
				break;
			case COLORBLENDMETHOD_MODULATE2X:
				Prop = L"Modulate2X";
				break;
			case COLORBLENDMETHOD_MODULATE4X:
				Prop = L"Modulate4X";
				break;
			default:
				throw fcyException("lambda [&](std::wstring&, const COLORBLENDMETHOD*)", "Value of COLORBLENDMETHOD is invalid.");
			}
		},
		[&](const std::wstring& Prop, COLORBLENDMETHOD* Value){
			if(_wcsicmp(Prop.c_str(), L"default")==0)
				*Value = COLORBLENDMETHOD_DEFAULT;
			else if(_wcsicmp(Prop.c_str(), L"add")==0)
				*Value = COLORBLENDMETHOD_ADD;
			else if(_wcsicmp(Prop.c_str(), L"subtract")==0)
				*Value = COLORBLENDMETHOD_SUBTRACT;
			else if(_wcsicmp(Prop.c_str(), L"modulate")==0)
				*Value = COLORBLENDMETHOD_MODULATE;
			else if(_wcsicmp(Prop.c_str(), L"modulate2X")==0)
				*Value = COLORBLENDMETHOD_MODULATE2X;
			else if(_wcsicmp(Prop.c_str(), L"modulate4X")==0)
				*Value = COLORBLENDMETHOD_MODULATE4X;
			else
				throw fcyException("lambda [&](const std::wstring&, COLORBLENDMETHOD*)", "Property string is not correct.");
		}
	);

	// 注册属性
	RegisterProperty(L"Image", &m_Image_Accessor);
	RegisterProperty(L"BlendColor", &m_BlendColor_Accessor);
	RegisterProperty(L"FillMethod", &m_ImageFillMethod_Accessor);
	RegisterProperty(L"ColorBlendMethod", &m_ColorBlendMethod_Accessor);

	// 注册事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiImageBox::OnStyleChanged);
}

fuiImageBox::~fuiImageBox()
{}

void fuiImageBox::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiSprite> tImageSprite;

	FUIGETRESANDCHECK(m_Image, tImageSprite, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiImageBox::OnStyleChanged");

	m_pImageSprite = tImageSprite;
}

void fuiImageBox::DrawImage(fuiGraphics* pGraph, fuiSprite* pSprite, const fcyColor& BlendColor, IMAGEFILLMETHOD Method, COLORBLENDMETHOD ColorMethod)
{
	if(pSprite && BlendColor.a != 0)
	{
		f2dSprite* p = pSprite->GetSprite();
		p->SetColor(BlendColor);

		F2DGRAPH2DBLENDTYPE tOrgType = pGraph->GetGraphics()->GetColorBlendType();
		switch(ColorMethod)
		{
		case COLORBLENDMETHOD_ADD:
			pGraph->GetGraphics()->SetColorBlendType(F2DGRAPH2DBLENDTYPE_ADD);
			break;
		case COLORBLENDMETHOD_SUBTRACT:
			pGraph->GetGraphics()->SetColorBlendType(F2DGRAPH2DBLENDTYPE_SUBTRACT);
			break;
		case COLORBLENDMETHOD_MODULATE:
			pGraph->GetGraphics()->SetColorBlendType(F2DGRAPH2DBLENDTYPE_MODULATE);
			break;
		case COLORBLENDMETHOD_MODULATE2X:
			pGraph->GetGraphics()->SetColorBlendType(F2DGRAPH2DBLENDTYPE_MODULATE2X);
			break;
		case COLORBLENDMETHOD_MODULATE4X:
			pGraph->GetGraphics()->SetColorBlendType(F2DGRAPH2DBLENDTYPE_MODULATE4X);
			break;
		}

		switch(Method)
		{
		case IMAGEFILLMETHOD_LEFTTOP:
			{
				const fcyRect& tRect = p->GetTexRect();
				p->Draw(pGraph->GetGraphics(), fcyVec2(tRect.GetWidth() / 2.f, tRect.GetHeight() / 2.f));
			}
			break;
		case IMAGEFILLMETHOD_FIT:
			{
				float tScale = GetWidth() / p->GetTexRect().GetWidth();
				if(GetHeight() * tScale > p->GetTexRect().GetHeight())
					tScale = GetHeight() / p->GetTexRect().GetHeight();
				
				p->Draw(pGraph->GetGraphics(), fcyVec2(GetWidth() / 2.f, GetHeight() / 2.f), fcyVec2(tScale, tScale));
			}
			break;
		case IMAGEFILLMETHOD_STRETCH:
			p->Draw(pGraph->GetGraphics(),
				fcyVec2(GetWidth() / 2.f, GetHeight() / 2.f),
				fcyVec2(GetWidth() / p->GetTexRect().GetWidth(), GetHeight() / p->GetTexRect().GetHeight()));
			break;
		}

		if(ColorMethod != COLORBLENDMETHOD_DEFAULT)
			pGraph->GetGraphics()->SetColorBlendType(tOrgType);
	}
}

void fuiImageBox::Update(fDouble ElapsedTime)
{
	fuiControl::Update(ElapsedTime);
}

void fuiImageBox::Render(fuiGraphics* pGraph)
{
	DrawImage(pGraph, m_pImageSprite, m_BlendColor, m_ImageFillMethod, m_ColorBlendMethod);

	fuiControl::Render(pGraph);
}
