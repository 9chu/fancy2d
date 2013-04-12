#pragma once
#include "../fcyUIBase/fuiControl.h"

class fuiHProgressBar :
	public fuiControl
{
public:
	static const fCharW ControlName[];
protected:
	struct HProgressBarStyle
	{
		fcyVec2 Margin;
		fFloat Height;
		fcyVec2 BarMargin;
		fFloat BarHeight;
		fBool RightToLeft;
		f2dSprite* Left;
		f2dSprite* Center;
        f2dSprite* Right;
		f2dSprite* LeftBar;
		f2dSprite* CenterBar;
        f2dSprite* RightBar;
	};
protected:
	HProgressBarStyle m_Style;
	
	fcyRect m_Left;
	fcyRect m_Center;
	fcyRect m_Right;
	fcyRect m_LeftBar;
	fcyRect m_CenterBar;
	fcyRect m_RightBar;

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
	fuiHProgressBar(fuiContainer* pContainer, fFloat Value = 0.f);
	~fuiHProgressBar(void);
};
