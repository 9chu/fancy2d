////////////////////////////////////////////////////////////////////////////////
/// @file  fuiBorder.h
/// @brief fancyUI 边框
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 边框
////////////////////////////////////////////////////////////////////////////////
class fuiBorder :
	public fuiControl
{
protected: // 属性
	std::wstring m_BorderImage;
	fcyColor m_BlendColor;
	
	fuiPropertyAccessor<std::wstring> m_BorderImage_Accessor;
	fuiPropertyAccessor<fcyColor> m_BlendColor_Accessor;
protected: // 绘图资源
	fcyRefPointer<fuiBorderSprite> m_pBorderImageSprite;
	
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiBorder(fuiPage* pRootPage, const std::wstring& Name);
	~fuiBorder();
};
