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
	enum COLORBLENDMETHOD
	{
		COLORBLENDMETHOD_DEFAULT,
		COLORBLENDMETHOD_ADD,
		COLORBLENDMETHOD_SUBTRACT,
		COLORBLENDMETHOD_MODULATE,
		COLORBLENDMETHOD_MODULATE2X,
		COLORBLENDMETHOD_MODULATE4X
	};
protected: // 属性
	std::wstring m_Image;
	fcyColor m_BlendColor;
	IMAGEFILLMETHOD m_ImageFillMethod;
	COLORBLENDMETHOD m_ColorBlendMethod;
	
	fuiPropertyAccessor<std::wstring> m_Image_Accessor;
	fuiPropertyAccessor<fcyColor> m_BlendColor_Accessor;
	fuiPropertyAccessor<IMAGEFILLMETHOD> m_ImageFillMethod_Accessor;
	fuiPropertyAccessor<COLORBLENDMETHOD> m_ColorBlendMethod_Accessor;
protected: // 绘图资源
	fcyRefPointer<fuiSprite> m_pImageSprite;
	
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
protected:
	/// @brief 绘制图片
	/// @note  可以由派生类调用的渲染接口
	void DrawImage(fuiGraphics* pGraph, fuiSprite* pSprite, const fcyColor& BlendColor, IMAGEFILLMETHOD Method, COLORBLENDMETHOD ColorMethod);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiImageBox(fuiPage* pRootPage, const std::wstring& Name);
	~fuiImageBox();
};
