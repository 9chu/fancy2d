////////////////////////////////////////////////////////////////////////////////
/// @file  fuiLabelEx.h
/// @brief fancyUI 高级标签
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyUIBase/fuiControl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief UI 高级标签
////////////////////////////////////////////////////////////////////////////////
class fuiLabelEx :
	public fuiControl
{
	struct DrawCmd // 绘图命令
	{
		enum TYPE
		{
			TYPE_NULL,
			TYPE_COLOR, // 改变字体颜色
			TYPE_FONT   // 改变字体
		};

		TYPE Type;
		union
		{
			struct
			{
				fByte b;
				fByte g;
				fByte r;
				fByte a;
			} ColorData;
			f2dFontProvider* FontData;
		};
	};
protected: // 绘图数据
	std::vector<std::wstring> m_DrawText; // 绘制文本
	std::vector<DrawCmd> m_DrawCmd;       // 绘制命令
	std::unordered_map<std::wstring, fcyRefPointer<fuiFont>> m_FontList; // 所有引用的字体
	float m_LineHeight;                   // 行高
	float m_MaxAscender;                  // 最大上界
	fuInt m_TotalTextLength;              // 总文本数量
	float m_YOffset;                      // Y轴起笔偏移
	fcyVec2 m_PenEndPos;                  // 落笔位置

	// 字符显示速率
	fuInt m_CurDisplayCount;               // 当前显示文本数量
	float m_CharWait;                      // 当前等待时间
protected: // 属性
	std::wstring m_Text;
	std::wstring m_FontName;
	fcyColor m_FontColor;
	fcyRefPointer<fuiFont> m_Font;
	float m_ZValue;
	float m_CharDisplaySpeed;
	int m_TotalAlpha;  // 整体Alpha [0~255]

	fuiPropertyAccessor<std::wstring> m_Text_Accessor;
	fuiPropertyAccessor<std::wstring> m_FontName_Accessor;
	fuiPropertyAccessor<fcyColor> m_FontColor_Accessor;
	fuiPropertyAccessor<float> m_ZValue_Accessor;
	fuiPropertyAccessor<float> m_CharDisplaySpeed_Accessor;
	fuiPropertyAccessor<int> m_TotalAlpha_Accessor;
	fuiPropertyAccessor<float> m_YOffset_Accessor;          // Y轴起笔偏移

	// 只读属性
	fuiPropertyAccessor<float> m_LineHeight_Accessor;       // 行高
	fuiPropertyAccessor<float> m_MaxAscender_Accessor;      // 最大上界
	fuiPropertyAccessor<fcyVec2> m_PenEndPos_Accessor;      // 落笔位置
protected:
	fcyVec2 drawText(f2dGraphics2D* pGraph, const std::wstring& Text, int Count, f2dFontProvider* pFontProvider, const fcyColor& BlendColor, const fcyVec2& StartPos, float Margin = 2.f);
	fcyVec2 calcuTextEndDrawPos();
protected: // 事件
	void OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs);
	void OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs);
public: // 实现接口
	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph);
public:
	fuiLabelEx(fuiPage* pRootPage, const std::wstring& Name);
	~fuiLabelEx();
};
