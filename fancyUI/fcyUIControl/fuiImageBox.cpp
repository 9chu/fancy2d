#include "fuiImageBox.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiImageBox::fuiImageBox(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_ImageFillMethod(IMAGEFILLMETHOD_LEFTTOP)
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

	// 注册属性
	RegisterProperty(L"Image", &m_Image_Accessor);
	RegisterProperty(L"BlendColor", &m_BlendColor_Accessor);
	RegisterProperty(L"FillMethod", &m_ImageFillMethod_Accessor);

	// 注册事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiImageBox::OnStyleChanged);
}

fuiImageBox::~fuiImageBox()
{}

void fuiImageBox::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_pImageSprite = (fuiSprite*)GetControlStyle()->QueryRes(m_Image);

	if(!m_Image.empty() && !m_pImageSprite)
		throw fcyException("fuiImageBox::OnStyleChanged", "Res not found.");
	if(m_pImageSprite && m_pImageSprite->GetResType() != fuiRes::RESTYPE_SPRITE)
		throw fcyException("fuiImageBox::OnStyleChanged", "Res type error.");
}

void fuiImageBox::Update(fDouble ElapsedTime)
{
	fuiControl::Update(ElapsedTime);
}

void fuiImageBox::Render(fuiGraphics* pGraph)
{
	if(m_pImageSprite)
	{
		f2dSprite* p = m_pImageSprite->GetSprite();
		p->SetColor(m_BlendColor);

		switch(m_ImageFillMethod)
		{
		case IMAGEFILLMETHOD_LEFTTOP:
			p->Draw(pGraph->GetGraphics(), fcyVec2());
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
	}

	fuiControl::Render(pGraph);
}
