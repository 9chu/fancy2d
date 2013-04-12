#include "fuiBoard.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiBoard::ControlName[] = L"Board";

fuiBoard::fuiBoard(fuiContainer* pContainer)
	: fuiContainer(pContainer), m_bIn(false)
{}

fuiBoard::~fuiBoard(void)
{}

void fuiBoard::recalcuBoardRect(const fcyRect& UIRect)
{
	m_ClipRect.a.x = UIRect.a.x + m_Style.Margin.a.x;
	m_ClipRect.a.y = UIRect.a.y + m_Style.Margin.a.y;
	m_ClipRect.b.x = UIRect.b.x - m_Style.Margin.b.x;
	m_ClipRect.b.y = UIRect.b.y - m_Style.Margin.b.y;

	m_RectLeft = fcyRect(
		UIRect.a.x,
		UIRect.a.y + m_Style.BoardMargin.a.y,
		UIRect.a.x + m_Style.BoardMargin.a.x,
		UIRect.b.y - m_Style.BoardMargin.b.y
		);
	m_RectTop = fcyRect(
		UIRect.a.x + m_Style.BoardMargin.a.x, 
		UIRect.a.y,
		UIRect.b.x - m_Style.BoardMargin.b.x,
		UIRect.a.y + m_Style.BoardMargin.a.y
		);
	m_RectRight = fcyRect(
		UIRect.b.x - m_Style.BoardMargin.b.x, 
		UIRect.a.y + m_Style.BoardMargin.b.y,
		UIRect.b.x,
		UIRect.b.y - m_Style.BoardMargin.b.y
		);
	m_RectBottom = fcyRect(
		UIRect.a.x + m_Style.BoardMargin.a.x,
		UIRect.b.y - m_Style.BoardMargin.b.y,
		UIRect.b.x - m_Style.BoardMargin.b.x,
		UIRect.b.y
		);
	m_RectCenter = fcyRect(
		UIRect.a.x + m_Style.BoardMargin.a.x,
		UIRect.a.y + m_Style.BoardMargin.a.y,
		UIRect.b.x - m_Style.BoardMargin.b.x,
		UIRect.b.y - m_Style.BoardMargin.b.y
		);
	m_RectLT = fcyRect(
		UIRect.a.x,
		UIRect.a.y,
		UIRect.a.x + m_Style.BoardMargin.a.x,
		UIRect.a.y + m_Style.BoardMargin.a.y
		);
	m_RectRT = fcyRect(
		UIRect.b.x - m_Style.BoardMargin.b.x,
		UIRect.a.y,
		UIRect.b.x,
		UIRect.a.y + m_Style.BoardMargin.a.y
		);
	m_RectLB = fcyRect(
		UIRect.a.x,
		UIRect.b.y - m_Style.BoardMargin.b.y,
		UIRect.a.x + m_Style.BoardMargin.a.x,
		UIRect.b.y
		);
	m_RectRB = fcyRect(
		UIRect.b.x - m_Style.BoardMargin.b.x,
		UIRect.b.y - m_Style.BoardMargin.b.y,
		UIRect.b.x,
		UIRect.b.y
		);

	if(m_Style.Board)
	{
		float tTexWidth = (float)m_Style.Board->GetTexRect().GetWidth();
		float tTexHeight = (float)m_Style.Board->GetTexRect().GetHeight();
		m_RectCenterUV = fcyRect(
			m_Style.SpriteMargin.a.x / tTexWidth,
			m_Style.SpriteMargin.a.y / tTexHeight,
			(tTexWidth - m_Style.SpriteMargin.b.x) / tTexWidth,
			(tTexHeight - m_Style.SpriteMargin.b.y) / tTexHeight
			);
	}
}

fFloat fuiBoard::GetClientHeight(const fcyRect& Size)
{
	return Size.GetHeight() - m_Style.Margin.a.y - m_Style.Margin.b.y;
}

fFloat fuiBoard::GetClientWidth(const fcyRect& Size)
{
	return Size.GetWidth() - m_Style.Margin.a.x - m_Style.Margin.b.x;
}

void fuiBoard::SetEvt_OnMouseIn(const std::function<void(void)>& Func)
{
	m_Evt_OnMouseIn = Func;
}

void fuiBoard::SetEvt_OnMouseOut(const std::function<void(void)>& Func)
{
	m_Evt_OnMouseOut = Func;
}

void fuiBoard::OnRender(fuiRenderer* pRenderer)
{
	if(GetStyle() == NULL)
		return;

	f2dGraphics2D* pGraph = pRenderer->GetGraph();

	m_Style.Board->Draw(pGraph, m_RectLeft, fcyRect(0.f, m_RectCenterUV.a.y, m_RectCenterUV.a.x, m_RectCenterUV.b.y));
	m_Style.Board->Draw(pGraph, m_RectTop, fcyRect(m_RectCenterUV.a.x, 0.f, m_RectCenterUV.b.x, m_RectCenterUV.a.y));
	m_Style.Board->Draw(pGraph, m_RectRight, fcyRect(m_RectCenterUV.b.x, m_RectCenterUV.a.y, 1.f, m_RectCenterUV.b.y));
	m_Style.Board->Draw(pGraph, m_RectBottom, fcyRect(m_RectCenterUV.a.x, m_RectCenterUV.b.y, m_RectCenterUV.b.x, 1.f));
	m_Style.Board->Draw(pGraph, m_RectCenter, m_RectCenterUV);
	m_Style.Board->Draw(pGraph, m_RectLT, fcyRect(0.f, 0.f, m_RectCenterUV.a.x, m_RectCenterUV.a.y));
	m_Style.Board->Draw(pGraph, m_RectRT, fcyRect(m_RectCenterUV.b.x, 0.f, 1.f, m_RectCenterUV.a.y));
	m_Style.Board->Draw(pGraph, m_RectLB, fcyRect(0.f, m_RectCenterUV.b.y, m_RectCenterUV.a.x, 1.f));
	m_Style.Board->Draw(pGraph, m_RectRB, fcyRect(m_RectCenterUV.b.x, m_RectCenterUV.b.y, 1.f, 1.f));

	if(!pRenderer->PushClip(m_ClipRect))
		return;

	fuiContainer::OnRender(pRenderer);

	pRenderer->PopClip();
}

void fuiBoard::OnResized(fcyRect& NewSize)
{
	recalcuBoardRect(NewSize);
}

void fuiBoard::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiBoard::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiBoard::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiBoard::OnStyleChanged", "Node 'Style' not exsited.");
	fcyXmlNode* pNode_Sprite = pStyleNode->GetNodeByName(L"Sprite", 0);
	if(!pNode_Sprite)
		throw fcyException("fuiBoard::OnStyleChanged", "Node 'Sprite' not exsited.");

	// 获得参数
	fBool tFlag = true;
	BoardStyle tStyle;
	tFlag |= fuiHelper::AttributeToRect(pNode_Style, L"Margin", tStyle.Margin);
	tFlag |= fuiHelper::AttributeToRect(pNode_Style, L"BoardMargin", tStyle.BoardMargin);
	tFlag |= fuiHelper::AttributeToRect(pNode_Style, L"SpriteMargin", tStyle.SpriteMargin);
	tFlag |= fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Board", tStyle.Board);
	if(tFlag == false)
		throw fcyException("fuiBoard::OnStyleChanged", "Err while parse style.");

	fcyVec2 tMinSize(
		tStyle.BoardMargin.a.x + tStyle.BoardMargin.b.x,
		tStyle.BoardMargin.a.y + tStyle.BoardMargin.b.y
		);

	m_Style = tStyle;

	setMinSize(tMinSize);

	recalcuBoardRect(GetRect());
}

void fuiBoard::OnQuerySubControlOffset(fcyVec2& pOut)
{
	pOut = m_Style.Margin.a;
}

void fuiBoard::OnQueryControlName(wstring& Out)
{
	fuiContainer::OnQueryControlName(Out);

	Out += L",";
	Out += ControlName;
}

const fcyRect& fuiBoard::OnQueryClipRect()
{
	return m_ClipRect;
}

void fuiBoard::OnMouseLeave()
{
	fuiContainer::OnMouseLeave();

	if(m_bIn)
	{
		m_bIn = false;

		if(m_Evt_OnMouseOut)
			m_Evt_OnMouseOut();
	}
}

fBool fuiBoard::OnMouseMove(fFloat X, fFloat Y)
{
	fBool tRet = fuiContainer::OnMouseMove(X,Y);

	if(!m_bIn)
	{
		m_bIn = true;

		if(m_Evt_OnMouseIn)
			m_Evt_OnMouseIn();
	}

	return true;
}
