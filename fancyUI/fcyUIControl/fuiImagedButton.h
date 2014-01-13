////////////////////////////////////////////////////////////////////////////////
/// @file  fuiImagedButton.h
/// @brief fancyUI 图形按钮
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"
#include "fuiImageBox.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 图形按钮
////////////////////////////////////////////////////////////////////////////////
class fuiImagedButton :
	public fuiImageBox
{
protected: // 属性
	std::wstring m_ActiveImage;
	std::wstring m_ClickImage;
	fcyRect m_MouseEventMargin;

	fuiPropertyAccessor<std::wstring> m_ActiveImage_Accessor;
	fuiPropertyAccessor<std::wstring> m_ClickImage_Accessor;
	fuiPropertyAccessor<fcyRect> m_MouseEventMargin_Accessor;
protected: // 绘图资源
	bool m_bIsMoveIn;
	bool m_bIsMouseDown;

	fcyRefPointer<fuiSprite> m_pActiveImageSprite;
	fcyRefPointer<fuiSprite> m_pClickImageSprite;
	
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
protected: // 实现鼠标事件
	void OnMouseMove(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnMouseLeave(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnMouseLButtonDown(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnMouseLButtonUp(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 覆盖ImageBox实现
	void Render(fuiGraphics* pGraph);
	fBool HitTest(const fcyVec2& Pos);
public:
	fuiImagedButton(fuiPage* pRootPage, const std::wstring& Name);
	~fuiImagedButton();
};
