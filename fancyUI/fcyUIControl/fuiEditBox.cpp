#include "fuiEditBox.h"

#include "../fcyUIBase/fuiContainer.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiEditBox::ControlName[] = L"EditBox";

fuiEditBox::fuiEditBox(fuiContainer* pContainer)
	: fuiControl(pContainer),
	m_FontHeight(0.f), m_DrawPosY(0.f), m_PointerX(0.f), m_PointerLeftTextWidth(0.f), m_PointerLeftCount(0),
	m_bFocus(false), m_bShowPointer(false), m_PointerTimer(0.f)
{
	SetClip(true);
}

fuiEditBox::~fuiEditBox(void)
{}

void fuiEditBox::resizeRect(const fcyRect& Rect)
{
	// 计算绘制y值
	m_DrawPosY = Rect.GetCenter().y + m_FontHeight / 2.f;
}

void fuiEditBox::setPointerPos(fuInt TextPos)
{
	fcyVec2 tStart;
	fcyVec2 tEnd;

	if(m_Style.pFont)
		m_Style.pFont->DrawTextW(NULL, m_Text.c_str(), TextPos, tStart, &tEnd);
	
	m_PointerLeftTextWidth = tEnd.x - tStart.x;
	m_PointerLeftCount = TextPos;
}

fuInt fuiEditBox::locatePos(float X, float& PointerPos)
{
	f2dFontRenderer* pFont = m_Style.pFont;
	if(!pFont)
		return 0;

	fuInt tPos = m_PointerLeftCount;
	float tPointerX = m_PointerX;

	if(X < tPointerX)
	{
		while(tPos)
		{
			fCharW tChar = m_Text[tPos - 1];
			
			fcyVec2 tStart;
			fcyVec2 tEnd;
			float tLen;
			pFont->DrawTextW(NULL, &tChar, 1, tStart, &tEnd);
			tLen = tEnd.x - tStart.x;
			
			if(X + tLen / 2.f < tPointerX)
			{
				tPointerX -= tLen;
				tPos--;
			}
			else
				break;
		}
	}
	else
	{
		while(tPos < m_Text.size())
		{
			fCharW tChar = m_Text[tPos];
			
			fcyVec2 tStart;
			fcyVec2 tEnd;
			float tLen;
			pFont->DrawTextW(NULL, &tChar, 1, tStart, &tEnd);
			tLen = tEnd.x - tStart.x;
			
			tPointerX += tLen;
			tPos++;
			if(X + tLen / 2.f <= tPointerX)
			{
				tPos--;
				tPointerX -= tLen;
				break;
			}
		}
	}

	PointerPos = tPointerX;
	return tPos;
}

void fuiEditBox::adjustPointer()
{
	if(m_PointerX < m_Style.PointerSize.x + 5.f)
		m_PointerX = m_Style.PointerSize.x + 5.f;
	else if(m_PointerX > GetRect().GetWidth() - m_Style.PointerSize.x)
		m_PointerX = GetRect().GetWidth() - m_Style.PointerSize.x;
}

const std::wstring& fuiEditBox::GetText()
{
	return m_Text;
}

void fuiEditBox::SetText(fcStrW Text)
{
	m_Text = Text;
	m_PointerLeftTextWidth = 0.f;
	m_PointerLeftCount = 0;
	m_PointerX = 0.f;

	adjustPointer();
}

void fuiEditBox::SetEvt_OnEnter(const std::function<void(void)>& Func)
{
	m_Evt_OnEnter = Func;
}

void fuiEditBox::OnRender(fuiRenderer* pRenderer)
{
	if(!m_Style.pFont)
		return;

	m_Style.pFont->SetColor(m_Style.FontColor);

	float tLeft = GetRect().a.x;

	// 绘制左侧
	m_Style.pFont->DrawTextW(
		pRenderer->GetGraph(), 
		m_Text.c_str(), 
		m_PointerLeftCount, 
		fcyVec2(tLeft + m_PointerX - m_PointerLeftTextWidth, m_DrawPosY),
		NULL);

	// 绘制光标
	if(m_Style.pPointer && m_bShowPointer)
	{
		fcyRect tRect(tLeft + m_PointerX - m_Style.PointerSize.x / 2.f, m_DrawPosY - m_FontHeight / 2 - m_Style.PointerSize.y / 2,
			tLeft + m_PointerX + m_Style.PointerSize.x / 2.f, m_DrawPosY - m_FontHeight / 2 + m_Style.PointerSize.y / 2);

		m_Style.pPointer->Draw(pRenderer->GetGraph(), tRect);
	}

	// 绘制右侧
	m_Style.pFont->DrawTextW(
		pRenderer->GetGraph(), 
		&m_Text[m_PointerLeftCount], 
		-1, 
		fcyVec2(tLeft + m_PointerX, m_DrawPosY),
		NULL);
}

void fuiEditBox::OnUpdate(fDouble ElapsedTime)
{
	if(m_bFocus)
	{
		m_PointerTimer += (float)ElapsedTime;
		if(m_Style.PointerFreq <= m_PointerTimer)
		{
			m_PointerTimer = 0.f;
			m_bShowPointer = !m_bShowPointer;
		}
	}
}

void fuiEditBox::OnResized(fcyRect& NewSize)
{
	resizeRect(NewSize);
}

void fuiEditBox::OnStyleChanged(fcStrW NewStyleName)
{
	fuiContext* pContext = NULL;

	if(!GetParent() || !(pContext = GetParent()->GetContext()))
		throw fcyException("fuiEditBox::OnStyleChanged", "Invalid parent or context pointer.");

	// 获得样式并计算参数
	fcyXmlNode* pStyleNode = pContext->GetStyle(ControlName, NewStyleName);
	if(!pStyleNode)
		throw fcyException("fuiEditBox::OnStyleChanged", "Style not exsited.");

	// 获得参数节点
	fcyXmlNode* pNode_Style = pStyleNode->GetNodeByName(L"Style", 0);
	if(!pNode_Style)
		throw fcyException("fuiEditBox::OnStyleChanged", "Node 'Style' not exsited.");

	fcyXmlNode* pNode_Sprite = pStyleNode->GetNodeByName(L"Sprite", 0);
	if(!pNode_Sprite)
		throw fcyException("fuiEditBox::OnStyleChanged", "Node 'Sprite' not exsited.");

	// 获得参数
	EditBoxStyle tStyle;
	fcStrW tFontName;
	tFontName = pNode_Style->GetAttribute(L"Font");
	tStyle.pFont = pContext->GetFont(tFontName);
	tStyle.PointerFreq = (float)_wtof(pNode_Style->GetAttribute(L"PointerFreq"));
	if(!fuiHelper::AttributeToVec2(pNode_Style, L"PointerSize", tStyle.PointerSize))
		throw fcyException("fuiEditBox::OnStyleChanged", "Attribute PointerSize parse failed.");
	if(!fuiHelper::AttributeToSprite(pContext, pNode_Sprite, L"Pointer", tStyle.pPointer))
		throw fcyException("fuiEditBox::OnStyleChanged", "Sprite create failed.");
	if(!tStyle.pFont)
		throw fcyException("fuiEditBox::OnStyleChanged", "Font not exsited.");
	swscanf_s(pNode_Style->GetAttribute(L"FontColor"), L"%x", &tStyle.FontColor);

	m_FontHeight = tStyle.pFont->GetLineHeight();

	m_Style = tStyle;
	
	setPointerPos(m_PointerLeftCount);
	resizeRect(GetRect());

	m_PointerX = 0.f;
	adjustPointer();
}

void fuiEditBox::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out += L",";
	Out += ControlName;
}

fBool fuiEditBox::OnGetFocus()
{
	m_bFocus = true;
	m_PointerTimer = 0.f;
	m_bShowPointer = true;
	return true;
}

void fuiEditBox::OnLostFocus()
{
	m_bFocus = false;
	m_PointerTimer = 0.f;
	m_bShowPointer = false;
}

fBool fuiEditBox::OnMouseMove(fFloat X, fFloat Y)
{
	return true;
}

void fuiEditBox::OnLMouseUp(fFloat X, fFloat Y)
{
}

fBool fuiEditBox::OnLMouseDown(fFloat X, fFloat Y)
{
	GetFocus();

	float tPointer;

	// 计算击中哪个文字
	m_PointerLeftCount = locatePos(X, tPointer);
	m_PointerX = tPointer;
	setPointerPos(m_PointerLeftCount);

	m_PointerTimer = 0.f;
	m_bShowPointer = true;

	return true;
}

void fuiEditBox::OnKeyUp(F2DINPUTKEYCODE KeyCode)
{
}

void fuiEditBox::OnKeyDown(F2DINPUTKEYCODE KeyCode)
{
	f2dFontRenderer* pFont = m_Style.pFont;
	if(!pFont)
		return;

	m_PointerTimer = 0.f;
	m_bShowPointer = true;

	switch(KeyCode)
	{
	case F2DINPUTKEYCODE_RIGHT:
		if(m_PointerLeftCount < m_Text.size())
		{
			fCharW tChar = m_Text[m_PointerLeftCount];
			
			fcyVec2 tStart;
			fcyVec2 tEnd;
			float tLen;
			pFont->DrawTextW(NULL, &tChar, 1, tStart, &tEnd);
			tLen = tEnd.x - tStart.x;

			m_PointerLeftCount++;
			m_PointerX += tLen;
			setPointerPos(m_PointerLeftCount);

			adjustPointer();
		}
		break;
	case F2DINPUTKEYCODE_LEFT:
		if(m_PointerLeftCount)
		{
			fCharW tChar = m_Text[m_PointerLeftCount - 1];
			
			fcyVec2 tStart;
			fcyVec2 tEnd;
			float tLen;
			pFont->DrawTextW(NULL, &tChar, 1, tStart, &tEnd);
			tLen = tEnd.x - tStart.x;

			m_PointerLeftCount--;
			m_PointerX -= tLen;
			setPointerPos(m_PointerLeftCount);

			adjustPointer();
		}
		break;
	case F2DINPUTKEYCODE_BACK:
		if(m_PointerLeftCount)
		{
			fCharW tChar = m_Text[m_PointerLeftCount - 1];

			fcyVec2 tStart;
			fcyVec2 tEnd;
			float tLen;
			pFont->DrawTextW(NULL, &tChar, 1, tStart, &tEnd);
			tLen = tEnd.x - tStart.x;

			m_Text.erase(m_Text.begin() + (m_PointerLeftCount - 1));
			m_PointerLeftCount--;
			m_PointerX -= tLen;
			setPointerPos(m_PointerLeftCount);
			adjustPointer();
		}
		break;
	case F2DINPUTKEYCODE_RETURN:
		if(m_Evt_OnEnter)
			m_Evt_OnEnter();
		break;
	}
}

void fuiEditBox::OnCharInput(fCharW CharCode)
{
	if(iswcntrl(CharCode))
		return;
	if(CharCode == L'\n' || CharCode == L'\r')
		return;

	m_PointerTimer = 0.f;
	m_bShowPointer = true;

	m_Text.insert(m_Text.begin() + m_PointerLeftCount, CharCode);

	f2dFontRenderer* pFont = m_Style.pFont;
	if(!pFont)
		return;

	fcyVec2 tStart;
	fcyVec2 tEnd;
	float tLen;
	pFont->DrawTextW(NULL, &CharCode, 1, tStart, &tEnd);
	tLen = tEnd.x - tStart.x;

	m_PointerX += tLen;
	m_PointerLeftCount++;

	setPointerPos(m_PointerLeftCount);
	adjustPointer();
}
