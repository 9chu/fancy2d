#include "fuiLabel.h"

#include "../fcyUIBase/fuiContainer.h"

#include <fcyMisc/fcyStringHelper.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiLabel::ControlName[] = L"Label";

fuiLabel::fuiLabel(fuiContainer* pContainer)
	: fuiControl(pContainer), m_FontHeight(0.f)
{
	SetClip(true);
}

fuiLabel::~fuiLabel(void)
{}

void fuiLabel::recalcuPos(const fcyRect& UIRect)
{
	// 水平绘制参考点
	switch(m_Style.HStyle)
	{
	case LABELHSTYLE_CENTER: // 居中
		m_RefPos.x = UIRect.GetCenter().x;
		break;
	case LABELHSTYLE_RIGHT: // 右对齐
		m_RefPos.x = UIRect.b.x;
		break;
	default: // 左对齐
		m_RefPos.x = UIRect.a.x;
	}

	// 垂直绘制参考点
	switch(m_Style.VStyle)
	{
	case LABELVSTYLE_CENTER:
		{
			float tTotalHeight = m_Caption.size() * m_FontHeight;
			m_RefPos.y = UIRect.GetCenter().y - tTotalHeight / 2.f + m_FontHeight;
		}
		break;
	case LABELVSTYLE_BOTTOM:
		{
			float tTotalHeight = m_Caption.size() * m_FontHeight;
			m_RefPos.y = UIRect.b.y - tTotalHeight + m_FontHeight - 2.f; // 保留2个像素边界
		}
		break;
	default:
		m_RefPos.y = UIRect.a.y + m_FontHeight;
	}
}

void fuiLabel::recalcuLineLength()
{
	m_LineLength.clear();

	vector<wstring>::iterator i = m_Caption.begin();
	for(; i != m_Caption.end(); i++)
	{
		// 计算行宽
		float tLen = 0;
		if(m_Style.pFont)
		{
			fcyVec2 tPos;
			fcyVec2 tEnd;
			m_Style.pFont->DrawTextW(NULL, (*i).c_str(), -1, tPos, &tEnd);
			tLen = tEnd.x - tPos.x;
		}
		m_LineLength.push_back(tLen);
	}
}

void fuiLabel::GetCaption(wstring& Out)
{
	Out.clear();

	vector<wstring>::iterator i = m_Caption.begin();
	for(; i != m_Caption.end(); i++)
	{
		if(!Out.empty())
			Out += L"\n";
		Out += (*i);
	}
}

void fuiLabel::SetCaption(fcStrW Text)
{
	fcyStringHelper::StringSplit(Text, L"\n", false, m_Caption);

	vector<wstring>::iterator i = m_Caption.begin();
	for(; i != m_Caption.end(); i++)
	{
		// 处理\r
		if((*i).length())
		{
			if((*i).back() == L'\r')
				(*i).pop_back();
		}
	}

	recalcuLineLength();
	recalcuPos(GetRect());
}

fuInt fuiLabel::GetLineCount()
{
	return m_Caption.size();
}

fcyColor fuiLabel::GetFontColor()
{
	return m_Style.FontColor;
}

void fuiLabel::SetFontColor(const fcyColor& Color)
{
	m_Style.FontColor = Color;
}

void fuiLabel::OnRender(fuiRenderer* pRenderer)
{
	if(!m_Style.pFont)
		return;

	f2dGraphics2D* pGraph = pRenderer->GetGraph();

	fcyVec2 tDrawPos = m_RefPos;

	for(fuInt i = 0; i<m_Caption.size(); i++)
	{
		switch(m_Style.HStyle)
		{
		case LABELHSTYLE_CENTER: // 居中
			tDrawPos.x = m_RefPos.x - m_LineLength[i] / 2;
			break;
		case LABELHSTYLE_RIGHT: // 右对齐
			tDrawPos.x = m_RefPos.x - m_LineLength[i];
			break;
		default: // 左对齐
			tDrawPos.x = m_RefPos.x;
		}

		// 绘制
		m_Style.pFont->SetColor(m_Style.FontColor);
		m_Style.pFont->DrawTextW(pGraph, m_Caption[i].c_str(), tDrawPos);

		tDrawPos.y += m_Style.pFont->GetLineHeight();
	}
}

void fuiLabel::OnResized(fcyRect& NewSize)
{
	recalcuPos(NewSize);
}

void fuiLabel::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiLabel::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiLabel::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiLabel::OnStyleChanged", "Node 'Style' not exsited.");

	// 获得参数
	LabelStyle tStyle;
	fcStrW tFontName;
	tStyle.HStyle = (LABELHSTYLE)_wtoi(pNode_Style->GetAttribute(L"HStyle"));
	tStyle.VStyle = (LABELVSTYLE)_wtoi(pNode_Style->GetAttribute(L"VStyle"));
	tFontName = pNode_Style->GetAttribute(L"Font");
	tStyle.pFont = pContext->GetFont(tFontName);
	if(!tStyle.pFont)
		throw fcyException("fuiLabel::OnStyleChanged", "Font not exsited.");

	swscanf_s(pNode_Style->GetAttribute(L"FontColor"), L"%x", &tStyle.FontColor);
	m_FontHeight = tStyle.pFont->GetLineHeight();

	m_Style = tStyle;
	
	recalcuLineLength();
	recalcuPos(GetRect());
}

void fuiLabel::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out += L",";
	Out += ControlName;
}
