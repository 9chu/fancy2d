////////////////////////////////////////////////////////////////////////////////
/// @file  fuiSliderBar.h
/// @brief fancyUI 水平滑动条
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

#include "fuiImagedButton.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 水平滑动条
////////////////////////////////////////////////////////////////////////////////
class fuiHSliderBar :
	public fuiControl
{
protected: // 子控件
	fcyRefPointer<fuiImagedButton> m_pSliderButton;
protected: // 属性
	std::wstring m_BackBorderSprite;   // 背景精灵
	std::wstring m_FrontBorderSprite;  // 前景精灵
	fcyRect m_SliderBorder;            // 滑动条边界
	
	fuiPropertyAccessor<std::wstring> m_BackBorderSprite_Accessor;
	fuiPropertyAccessor<std::wstring> m_FrontBorderSprite_Accessor;
	fuiPropertyAccessor<fcyRect> m_SliderBorder;
protected: // 绘图数据
	fcyRefPointer<fuiBorderSprite> m_BackBorderSprite;
	fcyRefPointer<fuiBorderSprite> m_FrontBorderSprite;

	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiHSliderBar(fuiPage* pRootPage, const std::wstring& Name);
	~fuiHSliderBar();
};
