////////////////////////////////////////////////////////////////////////////////
/// @file  fuiEditBox.h
/// @brief fancyUI 编辑框
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 编辑框
////////////////////////////////////////////////////////////////////////////////
class fuiEditBox :
	public fuiControl
{
protected: // 属性
	std::wstring m_Text;
	std::wstring m_FontName;
	fcyColor m_FontColor;
	std::wstring m_CursorSprite;
	fcyColor m_BlendColor;
	std::wstring m_BorderSprite;
	std::wstring m_ActiveBorderSprite;
	fcyRect m_Margin;
	fcyVec2 m_OffsetToInputBox;
	float m_CursorDisplayInterval;
	fBool m_bFitCursor;

	fuiPropertyAccessor<fcyVec2> m_OffsetToInputBox_Accessor;
	fuiPropertyAccessor<std::wstring> m_Text_Accessor;
	fuiPropertyAccessor<std::wstring> m_FontName_Accessor;
	fuiPropertyAccessor<fcyColor> m_FontColor_Accessor;
	fuiPropertyAccessor<std::wstring> m_CursorSprite_Accessor;
	fuiPropertyAccessor<fcyColor> m_BlendColor_Accessor;
	fuiPropertyAccessor<std::wstring> m_BorderSprite_Accessor;
	fuiPropertyAccessor<std::wstring> m_ActiveBorderSprite_Accessor;
	fuiPropertyAccessor<fcyRect> m_Margin_Accessor;
	fuiPropertyAccessor<float> m_CursorDisplayInterval_Accessor;
	fuiPropertyAccessor<fBool> m_FitCursor_Accessor;
private:
	fBool m_bInText;

	float m_LeftTextWidth;

	fuInt m_CursorPos;

	float m_CursorLeft;     ///< @brief 保存光标到Margin左侧的距离
	fcyVec2 m_TextDrawAt;   ///< @brief 保存Text的笔触位置
	float m_CursorDrawAtY;  ///< @brief 光标绘制位置

	fBool m_bCursorVisiable; ///< @brief 光标是否可见
	
	double m_Timer;          ///< @brief 光标计时器

	void adjustPos(); // 计算位置
protected: // 绘图资源
	fcyRefPointer<fuiSprite> m_Cursor;
	fcyRefPointer<fuiBorderSprite> m_Border;
	fcyRefPointer<fuiBorderSprite> m_ActiveBorder;
	fcyRefPointer<fuiFont> m_Font;
	f2dFontProvider* m_pFontProvider;
	f2dFontRenderer* m_pFontRenderer;

	void OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnSizeChanged(fuiControl* pThis, fuiEventArgs* pArgs);

	void OnCharInput(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnKeyUp(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnKeyDown(fuiControl* pThis, fuiEventArgs* pArgs);

	void OnGetFocus(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnLostFocus(fuiControl* pThis, fuiEventArgs* pArgs);

	void OnMouseLDown(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiEditBox(fuiPage* pRootPage, const std::wstring& Name);
	~fuiEditBox();
};
