////////////////////////////////////////////////////////////////////////////////
/// @file  fuiProgressBar.h
/// @brief fancyUI 横向进度条
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 横向进度条
////////////////////////////////////////////////////////////////////////////////
class fuiHProgressBar :
	public fuiControl
{
protected: // 属性
	std::wstring m_BackSprite;
	std::wstring m_LeftImage;
	std::wstring m_RightImage;
	std::wstring m_CenterImage;
	fcyColor m_BlendColor;
	float m_MaxValue;
	float m_MinValue;
	float m_Value;
	fcyRect m_ProgressMargin;

	fuiPropertyAccessor<std::wstring> m_BackSprite_Accessor;
	fuiPropertyAccessor<std::wstring> m_LeftImage_Accessor;
	fuiPropertyAccessor<std::wstring> m_RightImage_Accessor;
	fuiPropertyAccessor<std::wstring> m_CenterImage_Accessor;
	fuiPropertyAccessor<fcyColor> m_BlendColor_Accessor;
	fuiPropertyAccessor<float> m_MaxValue_Accessor;
	fuiPropertyAccessor<float> m_MinValue_Accessor;
	fuiPropertyAccessor<float> m_Value_Accessor;
	fuiPropertyAccessor<fcyRect> m_ProgressMargin_Accessor;
protected: // 绘图资源
	fcyRefPointer<fuiBorderSprite> m_pBackSprite;
	fcyRefPointer<fuiSprite> m_pLeftImage;
	fcyRefPointer<fuiSprite> m_pRightImage;
	fcyRefPointer<fuiSprite> m_pCenterImage;
	float m_LeftImageWidth;
	float m_RightImageWidth;
	float m_CenterImageWidth;
	float m_Percent;
	
	void UpdateValue();

	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiHProgressBar(fuiPage* pRootPage, const std::wstring& Name);
	~fuiHProgressBar();
};
