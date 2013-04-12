#pragma once
#include "../fcyUIBase/fuiControl.h"

#include <functional>

class fuiEditBox :
	public fuiControl
{
private:
	static const fCharW ControlName[];
protected:
	struct EditBoxStyle
	{
		fcyColor FontColor;
		f2dFontRenderer* pFont;
		f2dSprite* pPointer;
		fcyVec2 PointerSize;
		float PointerFreq;

		EditBoxStyle()
			: pFont(NULL), pPointer(NULL) {}
	};
protected:
	EditBoxStyle m_Style;

	std::wstring m_Text;
	float m_FontHeight;
	float m_DrawPosY;
	float m_PointerX;
	float m_PointerLeftTextWidth;
	fuInt m_PointerLeftCount;

	fBool m_bFocus;
	fBool m_bShowPointer;
	float m_PointerTimer;

	// 回调
	std::function<void(void)> m_Evt_OnEnter;
protected:
	void resizeRect(const fcyRect& Rect);
	void setPointerPos(fuInt TextPos);
	fuInt locatePos(float X, float& PointerPos);
	void adjustPointer();
public:
	const std::wstring& GetText();
	void SetText(fcStrW Text);

	void SetEvt_OnEnter(const std::function<void(void)>& Func);
protected: // 覆写消息
	void OnRender(fuiRenderer* pRenderer);
	void OnUpdate(fDouble ElapsedTime); 
	void OnResized(fcyRect& NewSize);
	void OnStyleChanged(fcStrW NewStyleName);
	void OnQueryControlName(std::wstring& Out);

	fBool OnGetFocus();
	void OnLostFocus();
	fBool OnMouseMove(fFloat X, fFloat Y);
	void OnLMouseUp(fFloat X, fFloat Y);
	fBool OnLMouseDown(fFloat X, fFloat Y);
	void OnKeyUp(F2DINPUTKEYCODE KeyCode);
	void OnKeyDown(F2DINPUTKEYCODE KeyCode);
	void OnCharInput(fCharW CharCode);
public:
	fuiEditBox(fuiContainer* pContainer);
	~fuiEditBox(void);
};
