#pragma once
#include "../fcyUIBase/fuiControl.h"

#include <functional>

class fuiSpriteButton :
	public fuiControl
{
private:
	static const fCharW ControlName[];

	enum SPRITEBUTTONSTATE
	{
		SPRITEBUTTONSTATE_DEFAULT,
		SPRITEBUTTONSTATE_MOUSEON,
		SPRITEBUTTONSTATE_MOUSEDOWN
	};
	struct SpriteButtonStyle
	{
		fcyVec2 Size;
		f2dSprite* Default;
		f2dSprite* MouseOn;
		f2dSprite* MouseDown;
	};
protected:
	SpriteButtonStyle m_Style;
	SPRITEBUTTONSTATE m_MouseState;
	fcyRect m_SpriteRect;
	fcyColor m_BlendColor;
	fBool m_bIn;

	// 回调
	std::function<void(void)> m_Evt_OnClick;
	std::function<void(void)> m_Evt_OnMouseIn;
	std::function<void(void)> m_Evt_OnMouseOut;
private:
	void recalcuRect(const fcyRect& UIRect);
protected: // 覆写事件
	void OnRender(fuiRenderer* pRenderer);
	void OnMouseLeave();
	fBool OnMouseMove(fFloat X, fFloat Y);
	void OnLMouseUp(fFloat X, fFloat Y);
	fBool OnLMouseDown(fFloat X, fFloat Y);
	void OnResized(fcyRect& NewSize);
	void OnStyleChanged(fcStrW NewStyleName);
	void OnQueryControlName(std::wstring& Out);
public: // 方法
	fcyColor GetBlendColor();
	void SetBlendColor(const fcyColor& Color);
	void SetEvt_OnClick(const std::function<void(void)>& Func);
	void SetEvt_OnMouseIn(const std::function<void(void)>& Func);
	void SetEvt_OnMouseOut(const std::function<void(void)>& Func);
public:
	fuiSpriteButton(fuiContainer* pContainer);
	~fuiSpriteButton(void);
};
