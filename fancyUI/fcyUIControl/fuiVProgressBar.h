#pragma once
#include "../fcyUIBase/fuiControl.h"

class fuiVProgressBar :
	public fuiControl
{
public:
	static const fCharW ControlName[];
protected:
	struct VProgressBarStyle
	{
		fcyVec2 Margin;
		fFloat Width;
		fcyVec2 BarMargin;
		fFloat BarWidth;
		fBool TopToBottom;
		f2dSprite* Top;
		f2dSprite* Center;
        f2dSprite* Bottom;
		f2dSprite* TopBar;
		f2dSprite* CenterBar;
        f2dSprite* BottomBar;
	};
protected:
	VProgressBarStyle m_Style;
	
	fcyRect m_Top;
	fcyRect m_Center;
	fcyRect m_Bottom;
	fcyRect m_TopBar;
	fcyRect m_CenterBar;
	fcyRect m_BottomBar;

	fFloat m_Value;
protected:
	void recalcuRect(const fcyRect& UIRect);
protected: // ¸²Ð´ÊÂ¼þ
	void OnRender(fuiRenderer* pRenderer);
	void OnResized(fcyRect& NewSize);
	void OnStyleChanged(fcStrW NewStyleName);
	void OnQueryControlName(std::wstring& Out);
public:
	fFloat GetValue();
	void SetValue(fFloat Value);
public:
	fuiVProgressBar(fuiContainer* pContainer, fFloat Value = 0.f);
	~fuiVProgressBar(void);
};
