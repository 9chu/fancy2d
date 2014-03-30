#include "fuiIME.h"

#include "fuiExceptionMacro.h"

#include "../fcyUIBase/fuiPage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiIME::fuiIME(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), 
	m_bMultiLine(true), m_bHMode(false), m_ActiveIndex(0),
	m_pFontProvider(NULL), m_pFontRenderer(NULL), m_bInInput(false), m_bInComposition(false)
{
	// 默认属性
	SetVisible(false);
	SetLayerPriority(FUIIME_LAYERPRIORITY);

	// 设置访问器
	m_SingleLineSkin_Accessor = fuiPropertyAccessor<std::wstring>(
		&m_SingleLineSkin,
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
	m_MultiLineSkin_Accessor = fuiPropertyAccessor<std::wstring>(
		&m_MultiLineSkin,
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
	m_bMultiLine_Accessor = fuiPropertyAccessor<fBool>(&m_bMultiLine);
	m_bHMode_Accessor = fuiPropertyAccessor<fBool>(&m_bHMode);
	m_InputBoxOffset_Accessor = fuiPropertyAccessor<fcyVec2>(&m_InputBoxOffset);
	m_CompositionMargin_Accessor = fuiPropertyAccessor<fcyRect>(&m_CompositionMargin);
	m_CandidateMargin_Accessor = fuiPropertyAccessor<fcyRect>(&m_CandidateMargin);
	m_FontName_Accessor = fuiPropertyAccessor<wstring>(&m_FontName);
	m_FontColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_FontColor);
	m_ActiveFontColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_ActiveFontColor);
	m_CandidateFontColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_CandidateFontColor);

	// 注册属性
	RegisterProperty(L"SingleLineSkin", &m_SingleLineSkin_Accessor);
	RegisterProperty(L"MultiLineSkin", &m_MultiLineSkin_Accessor);
	RegisterProperty(L"MultiLine", &m_bMultiLine_Accessor);
	RegisterProperty(L"HMode", &m_bHMode_Accessor);
	RegisterProperty(L"InputBoxOffset", &m_InputBoxOffset_Accessor);
	RegisterProperty(L"CompositionMargin", &m_CompositionMargin_Accessor);
	RegisterProperty(L"CandidateMargin", &m_CandidateMargin_Accessor);
	RegisterProperty(L"FontName", &m_FontName_Accessor);
	RegisterProperty(L"FontColor", &m_FontColor_Accessor);
	RegisterProperty(L"ActiveFontColor", &m_ActiveFontColor_Accessor);
	RegisterProperty(L"CandidateFontColor", &m_CandidateFontColor_Accessor);

	// 设置事件
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiIME::OnStyleChanged);

	pRootPage->GetEvent(L"OnIMEClosed") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEClosed);
	pRootPage->GetEvent(L"OnIMEActivated") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEActivated);
	pRootPage->GetEvent(L"OnIMEStartComposition") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEStartComposition);
	pRootPage->GetEvent(L"OnIMEEndComposition") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEEndComposition);
	pRootPage->GetEvent(L"OnIMEComposition") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEComposition);
	pRootPage->GetEvent(L"OnIMEOpenCandidate") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEOpenCandidate);
	pRootPage->GetEvent(L"OnIMECloseCandidate") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMECloseCandidate);
	pRootPage->GetEvent(L"OnIMEChangeCandidate") += fuiDelegate::EventCallBack(this, &fuiIME::OnIMEChangeCandidate);

	pRootPage->GetEvent(L"OnTextInputStart") += fuiDelegate::EventCallBack(this, &fuiIME::OnTextInputStart);
	pRootPage->GetEvent(L"OnTextInputEnd") += fuiDelegate::EventCallBack(this, &fuiIME::OnTextInputEnd);
	pRootPage->GetEvent(L"OnTextInputPosChanged") += fuiDelegate::EventCallBack(this, &fuiIME::OnTextInputPosChanged);
}

fuiIME::~fuiIME()
{
}

void fuiIME::recalcuStr()
{
	if(!m_pFontRenderer)
	{
		m_CompStrSize = fcyRect();
		m_CandidateStrSize = fcyRect();

		return;
	}

	// 计算拼写字符串包围盒
	m_CompStrSize = m_pFontRenderer->MeasureString(m_CompositionStr.c_str());
	m_CompStrSize.b.x += 2.f;

	wstring tCandidateStr;
	if(m_bHMode)
	{
		// 水平模式
		for(fuInt i = 0; i<m_CandidateList.size(); ++i)
		{
			tCandidateStr += m_CandidateList[i];
			if(i < m_CandidateList.size() - 1)
				tCandidateStr += L' ';
		}
	}
	else
	{
		// 垂直模式
		for(fuInt i = 0; i<m_CandidateList.size(); ++i)
		{
			tCandidateStr += m_CandidateList[i];
			if(i < m_CandidateList.size() - 1)
				tCandidateStr += L'\n';
		}
	}

	// 计算包围盒
	m_CandidateStrSize = m_pFontRenderer->MeasureString(tCandidateStr.c_str());
	m_CandidateStrSize.b.x += 2.f;
	if(m_bHMode)
	{
		m_CandidateStrSize.a.y = 0;
		m_CandidateStrSize.b.y = m_pFontProvider->GetLineHeight();
	}
	else
	{
		m_CandidateStrSize.a.y = 0;
		m_CandidateStrSize.b.y = m_pFontProvider->GetLineHeight() * m_CandidateList.size();
	}

	m_CompStrSize.a.y = 0;
	m_CompStrSize.b.y = m_pFontProvider->GetLineHeight();
}

void fuiIME::readCandidate(f2dIMECandidateList* pList)
{
	// 读取候选词数据
	m_ActiveIndex = pList->GetCurIndex() - pList->GetPageStart();

	m_CandidateList.clear();
	m_CandidateList.reserve(pList->GetCount());
	fuInt tIndex = 1;
	for(fuInt i = pList->GetPageStart(); i<pList->GetCount(); ++i)
	{
		wstring tStr = fcyStringHelper::ToWideStr((int)tIndex);
		m_CandidateList.push_back(tStr + L". " + pList->GetCandidateStr(i));
		
		tIndex++;
		if(tIndex > pList->GetPageSize())
			break;
	}

	recalcuStr();
}

void fuiIME::showIME()
{
	SetVisible(m_bInInput && m_bInComposition);
}

void fuiIME::OnIMEClosed(fuiControl* pThis, fuiEventArgs* pArgs)
{}

void fuiIME::OnIMEActivated(fuiControl* pThis, fuiEventArgs* pArgs)
{}

void fuiIME::OnIMEStartComposition(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInComposition = true;
	showIME();
}

void fuiIME::OnIMEEndComposition(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInComposition = false;
	showIME();
}

void fuiIME::OnIMEComposition(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiStringEventArgs* pStrArgs = (fuiStringEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_STRING);

	m_CompositionStr = pStrArgs->GetString();

	recalcuStr();
}

void fuiIME::OnIMEOpenCandidate(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiIMECandidateEventArgs* pIMEArgs = (fuiIMECandidateEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_IMECANDIDATE);
	f2dIMECandidateList* pList = pIMEArgs->GetList();

	readCandidate(pList);
}

void fuiIME::OnIMECloseCandidate(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiIMECandidateEventArgs* pIMEArgs = (fuiIMECandidateEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_IMECANDIDATE);
	f2dIMECandidateList* pList = pIMEArgs->GetList();
	
	readCandidate(pList);
}

void fuiIME::OnIMEChangeCandidate(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiIMECandidateEventArgs* pIMEArgs = (fuiIMECandidateEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_IMECANDIDATE);
	f2dIMECandidateList* pList = pIMEArgs->GetList();

	readCandidate(pList);
}

void fuiIME::OnTextInputStart(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInInput = true;
	showIME();
}

void fuiIME::OnTextInputPosChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fuiPositionEventArgs* pPosArgs = (fuiPositionEventArgs*)pArgs->QueryEvent(fuiEventArgs::EVENTTYPE_POSITION);

	SetRect(fcyRect(pPosArgs->GetPos(), pPosArgs->GetPos()));
	showIME();
}

void fuiIME::OnTextInputEnd(fuiControl* pThis, fuiEventArgs* pArgs)
{
	m_bInInput = false;
	showIME();
}

void fuiIME::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiBorderSprite> tSingleLineSkin;
	fcyRefPointer<fuiBorderSprite> tMultiLineSkin;
	fcyRefPointer<fuiFont> tFont;

	FUIGETRESANDCHECK(m_SingleLineSkin, tSingleLineSkin, fuiBorderSprite, fuiRes::RESTYPE_BORDERSPRITE, "fuiIME::OnStyleChanged");
	FUIGETRESANDCHECK(m_MultiLineSkin, tMultiLineSkin, fuiBorderSprite, fuiRes::RESTYPE_BORDERSPRITE, "fuiIME::OnStyleChanged");
	FUIGETRESANDCHECK(m_FontName, tFont, fuiFont, fuiRes::RESTYPE_FONT, "fuiIME::OnStyleChanged");

	m_pSingleLineSkin = tSingleLineSkin;
	m_pMultiLineSkin = tMultiLineSkin;
	m_Font = tFont;

	m_pFontProvider = NULL;
	m_pFontRenderer = NULL;
	if(m_Font)
	{
		m_pFontProvider = m_Font->GetFontProvider();
		m_pFontRenderer = m_Font->GetFontRenderer();
	}

	recalcuStr();
}

void fuiIME::Update(fDouble ElapsedTime)
{
}

void fuiIME::Render(fuiGraphics* pGraph)
{
	fcyRect tRect = fcyRect(m_InputBoxOffset, m_InputBoxOffset);
	fcyVec2 tCandidatePos = m_InputBoxOffset;
	fcyVec2 tAbsolutPos = GetAbsolutePos();

	// 计算并绘制外框
	if(m_bMultiLine)
	{
		// 多行模式
		tRect.b += m_CompositionMargin.a;
		tRect.b += fcyVec2(m_CompStrSize.GetWidth(), m_CompStrSize.GetHeight());
		tRect.b += m_CompositionMargin.b;
		tRect.b.y += m_CandidateMargin.a.y + m_CandidateStrSize.GetHeight() + m_CandidateMargin.b.y;

		float tPosX;
		tPosX = m_InputBoxOffset.x + m_CandidateMargin.a.x + m_CandidateStrSize.GetWidth() + m_CandidateMargin.b.x;

		tRect.b.x = FCYMAX(tRect.b.x, tPosX);

		// 调整使之不越出屏幕
		fcyVec2 tOffset = pGraph->GetTopOffset();
		if(tAbsolutPos.x + tRect.GetWidth() > GetRoot()->GetRect().b.x)
			tOffset.x += GetRoot()->GetRect().b.x - (tAbsolutPos.x + tRect.GetWidth());
		if(tAbsolutPos.y + tRect.GetHeight() > GetRoot()->GetRect().b.y)
			tOffset.y += GetRoot()->GetRect().b.y - (tAbsolutPos.y + tRect.GetHeight());
		pGraph->PopOffset();
		pGraph->PushOffset(tOffset);

		if(m_pMultiLineSkin)
			m_pMultiLineSkin->Draw(pGraph, tRect);

		// 计算文本框
		if(m_pFontProvider && m_pFontRenderer)
		{
			fcyVec2 tCompTextPos = m_InputBoxOffset + m_CompositionMargin.a + fcyVec2(0.f, m_pFontProvider->GetAscender());
			
			m_pFontRenderer->SetColor(m_FontColor);
			m_pFontRenderer->DrawTextW(pGraph->GetGraphics(), m_CompositionStr.c_str(), tCompTextPos);

			// 计算候选框
			tCandidatePos += m_CompositionMargin.a;
			tCandidatePos.y += m_CompStrSize.GetHeight() + m_CompositionMargin.b.y + m_CandidateMargin.a.y + m_pFontProvider->GetAscender();
		}
	}
	else
	{
		// 单行模式
		tRect.b += m_CandidateMargin.a + 
			fcyVec2(m_CandidateStrSize.GetWidth(), m_CandidateStrSize.GetHeight()) + 
			m_CandidateMargin.b;
		
		// 调整使之不越出屏幕
		fcyVec2 tOffset = pGraph->GetTopOffset();
		if(tAbsolutPos.x + tRect.GetWidth() > GetRoot()->GetRect().b.x)
			tOffset.x += GetRoot()->GetRect().b.x - (tAbsolutPos.x + tRect.GetWidth());
		if(tAbsolutPos.y + tRect.GetHeight() > GetRoot()->GetRect().b.y)
			tOffset.y += GetRoot()->GetRect().b.y - (tAbsolutPos.y + tRect.GetHeight());
		pGraph->PopOffset();
		pGraph->PushOffset(tOffset);

		if(m_pSingleLineSkin)
			m_pSingleLineSkin->Draw(pGraph, tRect);

		// 计算文本框
		if(m_pFontProvider && m_pFontRenderer)
		{
			// 计算候选框
			tCandidatePos += m_CandidateMargin.a;
			tCandidatePos.y += m_pFontProvider->GetAscender();
		}
	}

	// 绘制候选框
	if(m_pFontProvider && m_pFontRenderer)
	{
		if(m_bHMode)
		{
			for(fuInt i = 0; i<m_CandidateList.size(); ++i)
			{
				if((int)i == m_ActiveIndex)
					m_pFontRenderer->SetColor(m_ActiveFontColor);
				else
					m_pFontRenderer->SetColor(m_CandidateFontColor);

				m_pFontRenderer->DrawTextW(pGraph->GetGraphics(), wstring(m_CandidateList[i] + L" ").c_str(), -1, tCandidatePos, &tCandidatePos);
			}
		}
		else
		{
			for(fuInt i = 0; i<m_CandidateList.size(); ++i)
			{
				if((int)i == m_ActiveIndex)
					m_pFontRenderer->SetColor(m_ActiveFontColor);
				else
					m_pFontRenderer->SetColor(m_CandidateFontColor);

				m_pFontRenderer->DrawTextW(pGraph->GetGraphics(), m_CandidateList[i].c_str(), tCandidatePos);
				tCandidatePos.y += m_pFontProvider->GetLineHeight();
			}
		}
	}
}
