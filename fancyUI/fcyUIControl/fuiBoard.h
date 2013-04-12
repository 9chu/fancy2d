#pragma once
#include "../fcyUIBase/fuiContainer.h"

#include <functional>

class fuiBoard :
	public fuiContainer
{
public:
	static const fCharW ControlName[];
protected:
	struct BoardStyle
	{
		fcyRect Margin;
		fcyRect BoardMargin;
		fcyRect SpriteMargin;
		f2dSprite* Board;

		BoardStyle()
			: Board(NULL) {}
	};
protected:
	BoardStyle m_Style;
	fcyRect m_ClipRect;

	fcyRect m_RectLeft;
	fcyRect m_RectTop;
	fcyRect m_RectRight;
	fcyRect m_RectBottom;
	fcyRect m_RectCenter;
	fcyRect m_RectLT;
	fcyRect m_RectRT;
	fcyRect m_RectLB;
	fcyRect m_RectRB;
	fcyRect m_RectCenterUV;

	fBool m_bIn;

	// 回调
	std::function<void(void)> m_Evt_OnMouseIn;
	std::function<void(void)> m_Evt_OnMouseOut;
private: // 私有方法
	void recalcuBoardRect(const fcyRect& UIRect);
public:
	fFloat GetClientHeight(const fcyRect& Size);
	fFloat GetClientWidth(const fcyRect& Size);

	void SetEvt_OnMouseIn(const std::function<void(void)>& Func);
	void SetEvt_OnMouseOut(const std::function<void(void)>& Func);
protected: // 覆写消息
	void OnRender(fuiRenderer* pRenderer);
	void OnResized(fcyRect& NewSize);
	void OnStyleChanged(fcStrW NewStyleName);
	void OnQuerySubControlOffset(fcyVec2& pOut);
	void OnQueryControlName(std::wstring& Out);
	const fcyRect& OnQueryClipRect();

	void OnMouseLeave();
	fBool OnMouseMove(fFloat X, fFloat Y);
public:
	fuiBoard(fuiContainer* pContainer);
	~fuiBoard(void);
};
