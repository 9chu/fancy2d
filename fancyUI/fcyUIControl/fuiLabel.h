////////////////////////////////////////////////////////////////////////////////
/// @file  fuiLabel.h
/// @brief fancyUI 标签
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 标签
////////////////////////////////////////////////////////////////////////////////
class fuiLabel :
	public fuiControl
{
public:
	enum HALIGNMODE
	{
		HALIGNMODE_LEFT,
		HALIGNMODE_CENTER,
		HALIGNMODE_RIGHT
	};
	enum VALIGNMODE
	{
		VALIGNMODE_TOP,
		VALIGNMODE_CENTER,
		VALIGNMODE_BOTTOM
	};
protected: // 度量数据
	std::vector<std::wstring> m_Lines;
	std::vector<float> m_LineWidth;
	float m_TextHeight;
protected: // 属性
	std::wstring m_Text;
	std::wstring m_FontName;
	fcyColor m_FontColor;
	HALIGNMODE m_HAlign;   // 水平对齐
	VALIGNMODE m_VAlign;   // 垂直对齐

	fuiPropertyAccessor<std::wstring> m_Text_Accessor;
	fuiPropertyAccessor<std::wstring> m_FontName_Accessor;
	fuiPropertyAccessor<fcyColor> m_FontColor_Accessor;
	fuiPropertyAccessor<HALIGNMODE> m_HAlign_Accessor;
	fuiPropertyAccessor<VALIGNMODE> m_VAlign_Accessor;
protected: // 绘图资源
	fcyRefPointer<fuiFont> m_Font;
	f2dFontProvider* m_pFontProvider;
	f2dFontRenderer* m_pFontRenderer;

	void OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiLabel(fuiPage* pRootPage, const std::wstring& Name);
	~fuiLabel();
};
