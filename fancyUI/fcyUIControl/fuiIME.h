////////////////////////////////////////////////////////////////////////////////
/// @file  fuiIME.h
/// @brief fancyUI 输入法控件
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyParser/fcyXml.h>

#include "fuiLabel.h"

#define FUIIME_LAYERPRIORITY 900.f

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI 输入法控件
////////////////////////////////////////////////////////////////////////////////
class fuiIME :
	public fuiControl
{
protected: // 属性
	std::wstring m_SingleLineSkin;  ///< @brief 单行皮肤
	std::wstring m_MultiLineSkin;   ///< @brief 多行皮肤
	fBool m_bMultiLine;             ///< @brief 多行模式
	fBool m_bHMode;                 ///< @brief 水平模式
	fcyVec2 m_InputBoxOffset;       ///< @brief 输入框距离光标的位移
	fcyRect m_CompositionMargin;    ///< @brief 拼写框边距
	fcyRect m_CandidateMargin;      ///< @brief 候选框边距
	std::wstring m_FontName;        ///< @brief 字体名称
	fcyColor m_FontColor;           ///< @brief 字体颜色
	fcyColor m_ActiveFontColor;     ///< @brief 激活字体颜色
	fcyColor m_CandidateFontColor;  ///< @brief 备选字体颜色

	// 访问器
	fuiPropertyAccessor<std::wstring> m_SingleLineSkin_Accessor;
	fuiPropertyAccessor<std::wstring> m_MultiLineSkin_Accessor;
	fuiPropertyAccessor<fBool> m_bMultiLine_Accessor;
	fuiPropertyAccessor<fBool> m_bHMode_Accessor;
	fuiPropertyAccessor<fcyVec2> m_InputBoxOffset_Accessor;
	fuiPropertyAccessor<fcyRect> m_CompositionMargin_Accessor;
	fuiPropertyAccessor<fcyRect> m_CandidateMargin_Accessor;
	fuiPropertyAccessor<std::wstring> m_FontName_Accessor;
	fuiPropertyAccessor<fcyColor> m_FontColor_Accessor;
	fuiPropertyAccessor<fcyColor> m_ActiveFontColor_Accessor;
	fuiPropertyAccessor<fcyColor> m_CandidateFontColor_Accessor;
protected: // 渲染数据
	fcyRefPointer<fuiBorderSprite> m_pSingleLineSkin;
	fcyRefPointer<fuiBorderSprite> m_pMultiLineSkin;
	fcyRefPointer<fuiFont> m_Font;
	f2dFontProvider* m_pFontProvider;
	f2dFontRenderer* m_pFontRenderer;
protected: // 输入法数据
	std::wstring m_CompositionStr;
	std::vector<std::wstring> m_CandidateList;
	fcyRect m_CompStrSize;
	fcyRect m_CandidateStrSize;
	fInt m_ActiveIndex;

	fBool m_bInInput;
	fBool m_bInComposition;
protected:
	void recalcuStr();
	void readCandidate(f2dIMECandidateList* pList);
	void showIME();
protected:
	void OnIMEClosed(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEActivated(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEStartComposition(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEEndComposition(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEComposition(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEOpenCandidate(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMECloseCandidate(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnIMEChangeCandidate(fuiControl* pThis, fuiEventArgs* pArgs);

	void OnTextInputStart(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnTextInputPosChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnTextInputEnd(fuiControl* pThis, fuiEventArgs* pArgs);

	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiIME(fuiPage* pRootPage, const std::wstring& Name);
	~fuiIME();
};
