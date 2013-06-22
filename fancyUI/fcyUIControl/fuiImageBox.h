////////////////////////////////////////////////////////////////////////////////
/// @file  fuiImageBox.h
/// @brief fancyUI 图像框
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 图像框
////////////////////////////////////////////////////////////////////////////////
class fuiImageBox :
	public fuiControl
{
public:
	enum IMAGEFILLMETHOD
	{
		IMAGEFILLMETHOD_LEFTTOP,
		IMAGEFILLMETHOD_FIT,
		IMAGEFILLMETHOD_STRETCH
	};
protected: // 属性
	std::wstring m_Image;
	fcyColor m_BlendColor;
	IMAGEFILLMETHOD m_ImageFillMethod;
	
	fuiPropertyAccessor<std::wstring> m_Image_Accessor;
	fuiPropertyAccessor<fcyColor> m_BlendColor_Accessor;
	fuiPropertyAccessor<IMAGEFILLMETHOD> m_ImageFillMethod_Accessor;
protected: // 绘图资源
	fcyRefPointer<fuiSprite> m_pImageSprite;
	
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiImageBox(fuiPage* pRootPage, const std::wstring& Name);
	~fuiImageBox();
};
