#pragma once
#include "../fcyUIBase/fuiControl.h"

#include <vector>
#include <string>

class fuiLabel :
	public fuiControl
{
private:
	static const fCharW ControlName[];
protected:
	enum LABELHSTYLE  // 水平样式
	{
		LABELHSTYLE_LEFT,
		LABELHSTYLE_CENTER,
		LABELHSTYLE_RIGHT
	};
	enum LABELVSTYLE  // 垂直样式
	{
		LABELVSTYLE_TOP,
		LABELVSTYLE_CENTER,
		LABELVSTYLE_BOTTOM
	};
	struct LabelStyle
	{
		LABELHSTYLE HStyle;
		LABELVSTYLE VStyle;
		fcyColor FontColor;
		f2dFontRenderer* pFont;

		LabelStyle()
			: pFont(NULL) {}
	};
protected:
	LabelStyle m_Style;
	float m_FontHeight;

	std::vector<std::wstring> m_Caption;  // 标题
	std::vector<float> m_LineLength;      // 单行长度

	fcyVec2 m_RefPos;  // 参考绘制位置
private:
	void recalcuPos(const fcyRect& UIRect);
	void recalcuLineLength();
public:
	void GetCaption(std::wstring& Out);
	void SetCaption(fcStrW Text);
	fuInt GetLineCount();
	fcyColor GetFontColor();
	void SetFontColor(const fcyColor& Color);
protected: // 覆写消息
	void OnRender(fuiRenderer* pRenderer);
	void OnResized(fcyRect& NewSize);
	void OnStyleChanged(fcStrW NewStyleName);
	void OnQueryControlName(std::wstring& Out);

	// 穿透并不接收任何消息
	fBool OnMouseMove(fFloat X, fFloat Y) { return false; }
	fBool OnLMouseDown(fFloat X, fFloat Y) { return false; }
	fBool OnRMouseDown(fFloat X, fFloat Y) { return false; }
	fBool OnMMouseDown(fFloat X, fFloat Y) { return false; }
public:
	fuiLabel(fuiContainer* pContainer);
	~fuiLabel(void);
};
