#include "fuiHProgressBar.h"

#include "fuiExceptionMacro.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiHProgressBar::fuiHProgressBar(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_MaxValue(100.f), m_MinValue(0.f), m_Value(100.f), m_Percent(1.f), m_LeftImageWidth(0), m_RightImageWidth(0), m_CenterImageWidth(0)
{
	// 默认属性
	SetClip(false);

	m_BackSprite_Accessor = fuiPropertyAccessor<wstring>(
		&m_BackSprite,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_BackSprite = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_LeftImage_Accessor = fuiPropertyAccessor<wstring>(
		&m_LeftImage,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_LeftImage = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_RightImage_Accessor = fuiPropertyAccessor<wstring>(
		&m_RightImage,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_RightImage = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_CenterImage_Accessor = fuiPropertyAccessor<wstring>(
		&m_CenterImage,
		fuiPropertyAccessorHelper<wstring>::DefaultGetter,
		[&](const std::wstring& PropData, std::wstring* Value) {
			m_CenterImage = PropData;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_BlendColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_BlendColor);
	m_MaxValue_Accessor = fuiPropertyAccessor<float>(
		&m_MaxValue,
		fuiPropertyAccessorHelper<float>::DefaultGetter,
		[&](const std::wstring& PropData, float* Value) {
			fuiPropertyAccessorHelper<float>::DefaultSetter(PropData, Value);

			UpdateValue();
			ExecEvent(L"OnValueChanged");
		}
	);
	m_MinValue_Accessor = fuiPropertyAccessor<float>(
		&m_MinValue,
		fuiPropertyAccessorHelper<float>::DefaultGetter,
		[&](const std::wstring& PropData, float* Value) {
			fuiPropertyAccessorHelper<float>::DefaultSetter(PropData, Value);

			UpdateValue();
			ExecEvent(L"OnValueChanged");
		}
	);
	m_Value_Accessor = fuiPropertyAccessor<float>(
		&m_Value,
		fuiPropertyAccessorHelper<float>::DefaultGetter,
		[&](const std::wstring& PropData, float* Value) {
			fuiPropertyAccessorHelper<float>::DefaultSetter(PropData, Value);

			UpdateValue();
			ExecEvent(L"OnValueChanged");
		}
	);
	m_ProgressMargin_Accessor = fuiPropertyAccessor<fcyRect>(&m_ProgressMargin);
	
	// 注册属性
	RegisterProperty(L"BackSprite",     &m_BackSprite_Accessor);
	RegisterProperty(L"LeftImage",      &m_LeftImage_Accessor);
	RegisterProperty(L"RightImage",     &m_RightImage_Accessor);
	RegisterProperty(L"CenterImage",    &m_CenterImage_Accessor);
	RegisterProperty(L"BlendColor",     &m_BlendColor_Accessor);
	RegisterProperty(L"MaxValue",       &m_MaxValue_Accessor);
	RegisterProperty(L"MinValue",       &m_MinValue_Accessor);
	RegisterProperty(L"Value",          &m_Value_Accessor);
	RegisterProperty(L"ProgressMargin", &m_ProgressMargin_Accessor);

	// 注册事件
	RegisterEvent(L"OnValueChanged");

	// 注册事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiHProgressBar::OnStyleChanged);
}

fuiHProgressBar::~fuiHProgressBar()
{}

void fuiHProgressBar::UpdateValue()
{
	// 计算进度百分比
	m_Percent = m_Value / (m_MaxValue - m_MinValue);
	if(m_Percent < 0.f)
		m_Percent = 0.f;
	else if(m_Percent > 1.f)
		m_Percent = 1.f;
}

void fuiHProgressBar::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiBorderSprite> tBackSprite;
	fcyRefPointer<fuiSprite> tLeftImageSprite;
	fcyRefPointer<fuiSprite> tRightImageSprite;
	fcyRefPointer<fuiSprite> tCenterImageSprite;

	FUIGETRESANDCHECK(m_BackSprite,  tBackSprite,        fuiBorderSprite, fuiRes::RESTYPE_BORDERSPRITE, "fuiHProgress::OnStyleChanged");
	FUIGETRESANDCHECK(m_LeftImage,   tLeftImageSprite,   fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiHProgress::OnStyleChanged");
	FUIGETRESANDCHECK(m_RightImage,  tRightImageSprite,  fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiHProgress::OnStyleChanged");
	FUIGETRESANDCHECK(m_CenterImage, tCenterImageSprite, fuiSprite, fuiRes::RESTYPE_SPRITE, "fuiHProgress::OnStyleChanged");

	m_pBackSprite  = tBackSprite;
	m_pLeftImage   = tLeftImageSprite;
	m_pRightImage  = tRightImageSprite;
	m_pCenterImage = tCenterImageSprite;

	if(m_pLeftImage)
		m_LeftImageWidth = m_pLeftImage->GetSprite()->GetTexRect().GetWidth();
	else
		m_LeftImageWidth = 0.f;
	if(m_pRightImage)
		m_RightImageWidth = m_pRightImage->GetSprite()->GetTexRect().GetWidth();
	else
		m_RightImageWidth = 0.f;
	if(m_pCenterImage)
		m_CenterImageWidth = m_pCenterImage->GetSprite()->GetTexRect().GetWidth();
	else
		m_CenterImageWidth = 0.f;
}

void fuiHProgressBar::Update(fDouble ElapsedTime)
{
	fuiControl::Update(ElapsedTime);
}

void fuiHProgressBar::Render(fuiGraphics* pGraph)
{
	if(m_pBackSprite)
		m_pBackSprite->Draw(pGraph, fcyRect(0.f, 0.f, GetWidth(), GetHeight()), m_BlendColor);
	if(m_pLeftImage && m_pCenterImage && m_pRightImage)
	{
		// 计算控件有效大小
		float tValidWidth = GetWidth() - m_ProgressMargin.a.x - m_ProgressMargin.b.x;
		float tValidHeight = GetHeight() - m_ProgressMargin.a.y - m_ProgressMargin.b.y;
		if(tValidWidth > 0.f && tValidHeight > 0.f)
		{
			float tTargetWidth = tValidWidth * m_Percent;
			float tCenterWidth = tValidWidth - m_LeftImageWidth - m_RightImageWidth;

			m_pLeftImage->GetSprite()->SetColor(m_BlendColor);
			m_pCenterImage->GetSprite()->SetColor(m_BlendColor);
			m_pRightImage->GetSprite()->SetColor(m_BlendColor);

			if(tTargetWidth > m_LeftImageWidth)
			{
				m_pLeftImage->GetSprite()->Draw(
					pGraph->GetGraphics(), 
					fcyRect(
						m_ProgressMargin.a.x,
						m_ProgressMargin.a.y,
						m_ProgressMargin.a.x + m_LeftImageWidth, 
						m_ProgressMargin.a.y + tValidHeight
					)
				);
				tTargetWidth -= m_LeftImageWidth;

				if(tTargetWidth > tCenterWidth)
				{
					m_pCenterImage->GetSprite()->Draw(
						pGraph->GetGraphics(), 
						fcyRect(
							m_ProgressMargin.a.x + m_LeftImageWidth,
							m_ProgressMargin.a.y,
							m_ProgressMargin.a.x + m_LeftImageWidth + tCenterWidth, 
							m_ProgressMargin.a.y + tValidHeight
						)
					);
					tTargetWidth -= tCenterWidth;
					
					m_pRightImage->GetSprite()->Draw(
						pGraph->GetGraphics(), 
						fcyRect(
							m_ProgressMargin.a.x + m_LeftImageWidth + tCenterWidth,
							m_ProgressMargin.a.y,
							m_ProgressMargin.a.x + m_LeftImageWidth + tCenterWidth + tTargetWidth, 
							m_ProgressMargin.a.y + tValidHeight
						),
						fcyRect(0.f, 0.f, tTargetWidth / m_RightImageWidth, 1.f)
					);
				}
				else
				{
					m_pCenterImage->GetSprite()->Draw(
						pGraph->GetGraphics(), 
						fcyRect(
							m_ProgressMargin.a.x + m_LeftImageWidth,
							m_ProgressMargin.a.y,
							m_ProgressMargin.a.x + m_LeftImageWidth + tTargetWidth, 
							m_ProgressMargin.a.y + tValidHeight
						),
						fcyRect(0.f, 0.f, tTargetWidth / tCenterWidth, 1.f)
					);
				}
			}
			else
			{
				m_pLeftImage->GetSprite()->Draw(
					pGraph->GetGraphics(), 
					fcyRect(
						m_ProgressMargin.a.x,
						m_ProgressMargin.a.y,
						m_ProgressMargin.a.x + tTargetWidth, 
						m_ProgressMargin.a.y + tValidHeight
					),
					fcyRect(0.f, 0.f, tTargetWidth / m_LeftImageWidth, 1.f)
				);
			}
		}
	}

	fuiControl::Render(pGraph);
}
