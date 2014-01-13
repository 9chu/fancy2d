#include "fuiLabelEx.h"

#include "fuiExceptionMacro.h"

#include <fcyParser/fcyLexicalParser.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiLabelEx::fuiLabelEx(fuiPage* pRootPage, const std::wstring& Name)
	: fuiControl(pRootPage, Name), m_LineHeight(0), m_ZValue(1.f), m_CharDisplaySpeed(0.03f), m_TotalAlpha(255), m_YOffset(0)
{
	// 默认参数
	m_bClip = true;

	// 设置访问器
	m_Text_Accessor = fuiPropertyAccessor<wstring>(
		&m_Text,
		[&](std::wstring& Prop, const std::wstring* Value)
		{
			Prop = *Value;
		},
		[&](const std::wstring& Prop, std::wstring* Value)
		{
			*Value = Prop;
			ExecEvent(L"OnTextChanged");
		}
	);
	m_FontName_Accessor = fuiPropertyAccessor<wstring>
	(
		&m_FontName,
		[&](std::wstring& Prop, const std::wstring* Value)
		{
			Prop = *Value;
		},
		[&](const std::wstring& Prop, std::wstring* Value)
		{
			*Value = Prop;

			ExecEvent(L"OnStyleChanged");
		}
	);
	m_FontColor_Accessor = fuiPropertyAccessor<fcyColor>(&m_FontColor);
	m_ZValue_Accessor = fuiPropertyAccessor<float>(&m_ZValue);
	m_CharDisplaySpeed_Accessor = fuiPropertyAccessor<float>(&m_CharDisplaySpeed);
	m_TotalAlpha_Accessor = fuiPropertyAccessor<int>(&m_TotalAlpha);
	m_YOffset_Accessor = fuiPropertyAccessor<float>(&m_YOffset);

	// 只读属性
	m_LineHeight_Accessor = fuiPropertyAccessor<float>(
		&m_LineHeight,
		fuiPropertyAccessorHelper<float>::DefaultGetter, // 可读
		fuiPropertyAccessor<float>::PropSetter()         // 只读
		);
	m_MaxAscender_Accessor = fuiPropertyAccessor<float>(
		&m_MaxAscender,
		fuiPropertyAccessorHelper<float>::DefaultGetter, // 可读
		fuiPropertyAccessor<float>::PropSetter()         // 只读
		);
	m_PenEndPos_Accessor = fuiPropertyAccessor<fcyVec2>(
		&m_PenEndPos,
		fuiPropertyAccessorHelper<fcyVec2>::DefaultGetter, // 可读
		fuiPropertyAccessor<fcyVec2>::PropSetter()         // 只读
		);

	// 注册属性
	RegisterProperty(L"Text", &m_Text_Accessor);
	RegisterProperty(L"FontName", &m_FontName_Accessor);
	RegisterProperty(L"FontColor", &m_FontColor_Accessor);
	RegisterProperty(L"ZValue", &m_ZValue_Accessor);
	RegisterProperty(L"CharDisplaySpeed", &m_CharDisplaySpeed_Accessor);
	RegisterProperty(L"TotalAlpha", &m_TotalAlpha_Accessor);
	RegisterProperty(L"YOffset", &m_YOffset_Accessor);
	RegisterProperty(L"LineHeight", &m_LineHeight_Accessor);
	RegisterProperty(L"MaxAscender", &m_MaxAscender_Accessor);
	RegisterProperty(L"PenEndPos", &m_PenEndPos_Accessor);

	// 注册事件
	RegisterEvent(L"OnTextChanged");

	// 设置事件
	GetEvent(L"OnTextChanged") += fuiDelegate::EventCallBack(this, &fuiLabelEx::OnTextChanged);
	GetEvent(L"OnStyleChanged") += fuiDelegate::EventCallBack(this, &fuiLabelEx::OnStyleChanged);
}

fuiLabelEx::~fuiLabelEx()
{}

fcyVec2 fuiLabelEx::drawText(f2dGraphics2D* pGraph, const std::wstring& Text, int Count, f2dFontProvider* pFontProvider, const fcyColor& BlendColor, const fcyVec2& StartPos, float Margin)
{
	if(!pGraph || !pFontProvider)
		return StartPos;

	// --- 准备顶点 ---
	fcyColor tBlendColor(BlendColor);
	tBlendColor.a = (fByte)(tBlendColor.a * (m_TotalAlpha / 255.f));
	f2dGraphics2DVertex tVerts[4] = 
	{
		{ 0.f, 0.f, m_ZValue, tBlendColor.argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, tBlendColor.argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, tBlendColor.argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, tBlendColor.argb, 0.f, 0.f }
	};

	// --- 计算需要绘制的数量 ---
	fcyVec2 tPos = StartPos;                        // 笔触位置

	// --- 绘制每一个字符 ---
	f2dTexture2D* pTex = pFontProvider->GetCacheTexture();
	if(!pTex)
		return StartPos;

	f2dGlyphInfo tInfo;
	for(int i = 0; i<Count; ++i)
	{
		// 换行处理
		if(Text[i] == L'\n')
		{
			tPos.x = Margin;
			tPos.y += m_LineHeight;
			continue;
		}

		// 取出文字
		if(FCYOK(pFontProvider->QueryGlyph(pGraph, Text[i], &tInfo)))
		{
			// 检查笔触是否越界
			if(tPos.x + tInfo.Advance.x > GetWidth() - Margin)
			{
				// 换行
				tPos.x = Margin;
				tPos.y += m_LineHeight;
			}

			// 拷贝贴图uv信息并进行翻转处理
			tVerts[0].u = tInfo.GlyphPos.a.x;
			tVerts[0].v = tInfo.GlyphPos.a.y;
			tVerts[1].u = tInfo.GlyphPos.b.x;
			tVerts[1].v = tInfo.GlyphPos.a.y;
			tVerts[2].u = tInfo.GlyphPos.b.x;
			tVerts[2].v = tInfo.GlyphPos.b.y;
			tVerts[3].u = tInfo.GlyphPos.a.x;
			tVerts[3].v = tInfo.GlyphPos.b.y;
			
			// 计算位置矩形
			tVerts[0].x = tPos.x - tInfo.BrushPos.x;
			tVerts[0].y = tPos.y - tInfo.BrushPos.y;
			tVerts[1].x = tVerts[0].x + tInfo.GlyphSize.x;
			tVerts[1].y = tVerts[0].y;
			tVerts[2].x = tVerts[0].x + tInfo.GlyphSize.x;
			tVerts[2].y = tVerts[0].y + tInfo.GlyphSize.y;
			tVerts[3].x = tVerts[0].x;
			tVerts[3].y = tVerts[2].y;
			
			// 绘图
			pGraph->DrawQuad(pTex, tVerts);

			tPos += tInfo.Advance;
		}
	}

	// 返回新的位置
	return tPos;
}

fcyVec2 fuiLabelEx::calcuTextEndDrawPos()
{
	f2dFontProvider* tProvider = NULL;
	if(m_Font)
		tProvider = m_Font->GetFontProvider();

	// 边距 = 2.f
	float tMargin = 2.f;
	fcyVec2 tStartPos(tMargin, m_MaxAscender);
	for(size_t i = 0; i<m_DrawText.size(); ++i)
	{
		int tCountToDraw = m_DrawText[i].length();

		// 计算一段文本长度
		f2dGlyphInfo tInfo;
		for(int j = 0; j<tCountToDraw; ++j)
		{
			// 换行处理
			if(m_DrawText[i][j] == L'\n')
			{
				tStartPos.x = tMargin;
				tStartPos.y += m_LineHeight;
				continue;
			}

			// 取出文字
			if(FCYOK(tProvider->QueryGlyph(NULL, m_DrawText[i][j], &tInfo)))
			{
				// 检查笔触是否越界
				if(tStartPos.x + tInfo.Advance.x > GetWidth() - tMargin)
				{
					// 换行
					tStartPos.x = tMargin;
					tStartPos.y += m_LineHeight;
				}
				tStartPos += tInfo.Advance;
			}
		}

		// 执行指令
		if(i != m_DrawText.size() - 1)
		{
			DrawCmd& tCmd = m_DrawCmd[i];
			switch(tCmd.Type)
			{
			case DrawCmd::TYPE_FONT:
				tProvider = tCmd.FontData;
				break;
			}
		}
	}

	return tStartPos;
}

void fuiLabelEx::OnTextChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	// 清空所有数据
	m_DrawText.clear();
	m_DrawCmd.clear();
	m_FontList.clear();
	if(m_Font)
	{
		m_LineHeight = m_Font->GetFontProvider()->GetLineHeight();
		m_MaxAscender = m_Font->GetFontProvider()->GetAscender();
	}
	else
	{
		m_LineHeight = 0.f;
		m_MaxAscender = 0.f;
	}
	m_TotalTextLength = 0;
	m_CurDisplayCount = 0;
	m_CharWait = 0.f;

	// 解析状态
	stack<fcyColor> tColorStack;  // 颜色栈
	stack<fcyRefPointer<fuiFont>> tFontStack;  // 字体栈

	// 解析文本
	fcyLexicalReader tReader(m_Text);
	wstring tLastStrBlock;
	while(!tReader.IsEOF())
	{
		fCharW tChar = tReader.ReadChar();

		switch (tChar)
		{
		case L'\\': // 处理转义
			tChar = tReader.ReadChar();
			switch (tChar)
			{
			case L'n': // 追加换行
				tLastStrBlock += L'\n';
				break;
			case L't': // 追加 \t
				tLastStrBlock += L'\t';
			case L'\\': // 转义 \ 
				tLastStrBlock += L'\\';
				break;
			case L'[': // 转义 [ 
				tLastStrBlock += L'[';
				break;
			default:   // 未识别
				tLastStrBlock += tChar;
				break;
			}
			break;
		case L'[': // 读取命令
			{
				while(1)
				{
					tReader.IgnoreSpace();

					// 读取命令名称
					bool bOK = false;
					wstring tCmdName;
					while(!bOK)
					{
						tChar = tReader.PeekChar();

						switch(tChar)
						{
						case L';':
						case L']':
						case L'=':
							bOK = true;
							break;
						default:
							tReader.ReadChar();
							tCmdName += tChar;
							break;
						}
					}

					// 检查命令类型
					if(tCmdName.length() > 0 && tCmdName[0] == L'/')
					{
						// 推入文本块
						m_DrawText.push_back(tLastStrBlock);
						tLastStrBlock.clear();

						// 堆栈回退
						if(tCmdName == L"/color")
						{
							fcyColor& obj = m_FontColor;

							if(!tColorStack.empty())
							{
								tColorStack.pop();

								if(!tColorStack.empty())
									obj = tColorStack.top();
							}
							
							DrawCmd tCmd;
							tCmd.Type = DrawCmd::TYPE_COLOR;
							*((fcyColor*)&tCmd.ColorData) = obj;
							
							m_DrawCmd.push_back(tCmd);
						}
						else if(tCmdName == L"/font")
						{
							fcyRefPointer<fuiFont> obj = m_Font;

							if(!tFontStack.empty())
							{
								tFontStack.pop();

								if(!tFontStack.empty())
									obj = tFontStack.top();
							}

							DrawCmd tCmd;
							tCmd.Type = DrawCmd::TYPE_FONT;
							if(obj != NULL)
								tCmd.FontData = obj->GetFontProvider();
							else
								tCmd.FontData = NULL;
							
							m_DrawCmd.push_back(tCmd);
						}
						else
							throw fcyException("fuiLabelEx::OnTextChanged", "Command '%s' not support.",
								fcyStringHelper::WideCharToMultiByte(tCmdName).c_str()
								);
					}
					else
					{
						// 匹配=
						tReader.Match(L'=', true);

						// 匹配"
						tReader.Match(L'"', true);

						// 读取参数
						wstring tParam;
						do
						{
							tChar = tReader.ReadChar();

							if(tChar != L'"')
								tParam += tChar;
						}
						while(tChar != L'"');

						// 推入文本块
						m_DrawText.push_back(tLastStrBlock);
						tLastStrBlock.clear();

						// 推入命令
						if(tCmdName == L"color")
						{
							fcyColor tColor;
							int tColorData[4];
							if(4 != swscanf_s(tParam.c_str(), L"%d,%d,%d,%d", &tColorData[0], &tColorData[1], &tColorData[2], &tColorData[3]))
							{
								throw fcyException("fuiLabelEx::OnTextChanged", "Param '%s' of command 'color' is not correct.",
									fcyStringHelper::WideCharToMultiByte(tParam).c_str()
									);
							}
							tColor.a = tColorData[0]; tColor.r = tColorData[1]; tColor.g = tColorData[2]; tColor.b = tColorData[3];

							tColorStack.push(tColor);

							DrawCmd tCmd;
							tCmd.Type = DrawCmd::TYPE_COLOR;
							*((fcyColor*)&tCmd.ColorData) = tColor;
							
							m_DrawCmd.push_back(tCmd);
						}
						else if(tCmdName == L"font")
						{
							fcyRefPointer<fuiFont> tFont;

							// 检查是否已经读取
							if(m_FontList.find(tParam) != m_FontList.end())
								tFont = m_FontList[tParam];
							else
							{
								// 加载字体
								tFont = (fuiFont*)GetControlStyle()->QueryRes(tParam);
								if(!tFont)
									throw fcyException("fuiLabelEx::OnTextChanged", "Font '%s' not found.",
										fcyStringHelper::WideCharToMultiByte(tParam).c_str()
										);

								// 获得行高
								if(tFont->GetFontProvider()->GetLineHeight() > m_LineHeight)
									m_LineHeight = tFont->GetFontProvider()->GetLineHeight();

								// 获得下届
								if(tFont->GetFontProvider()->GetAscender() > m_MaxAscender)
									m_MaxAscender = tFont->GetFontProvider()->GetAscender();

								m_FontList[tParam] = tFont;
							}

							// 推入堆栈
							tFontStack.push(tFont);

							DrawCmd tCmd;
							tCmd.Type = DrawCmd::TYPE_FONT;
							tCmd.FontData = tFont->GetFontProvider();
							
							m_DrawCmd.push_back(tCmd);
						}
						else
							throw fcyException("fuiLabelEx::OnTextChanged", "Command '%s' not support.",
								fcyStringHelper::WideCharToMultiByte(tCmdName).c_str()
								);
					}

					if(tReader.TryMatch(L';', true, true) == false)
					{
						tReader.Match(L']', true);
						break;
					}
				}
				break;
			}
		default:
			tLastStrBlock += tChar;
			break;
		}
	}

	m_DrawText.push_back(tLastStrBlock);	

	for(size_t i = 0; i<m_DrawText.size(); ++i)
	{
		m_TotalTextLength += m_DrawText[i].length();
	}

	if(m_CharDisplaySpeed == 0.f)
		m_CurDisplayCount = m_TotalTextLength;
	else
		m_CurDisplayCount = 0;

	m_PenEndPos = calcuTextEndDrawPos();
}

void fuiLabelEx::OnStyleChanged(fuiControl* pThis, fuiEventArgs* pArgs)
{
	fcyRefPointer<fuiFont> tFont;

	FUIGETRESANDCHECK(m_FontName, tFont, fuiFont, fuiRes::RESTYPE_FONT, "fuiLabelEx::OnStyleChanged");

	m_Font = tFont;

	// 重新布局
	OnTextChanged(pThis, pArgs);
}

void fuiLabelEx::Update(fDouble ElapsedTime)
{
	if(m_TotalTextLength > m_CurDisplayCount)
	{
		m_CharWait += (float)ElapsedTime;

		if(m_CharDisplaySpeed == 0.f)
			m_CurDisplayCount = m_TotalTextLength;
		else
		{
			m_CurDisplayCount += (int)(m_CharWait / m_CharDisplaySpeed);
			m_CharWait = fmod(m_CharWait, m_CharDisplaySpeed);

			if(m_CurDisplayCount > m_TotalTextLength)
			{
				m_CurDisplayCount = m_TotalTextLength;
			}
		}
	}
}

void fuiLabelEx::Render(fuiGraphics* pGraph)
{
	f2dFontProvider* tProvider = NULL;
	fcyColor tBlendColor = m_FontColor;
	if(m_Font)
		tProvider = m_Font->GetFontProvider();

	// 边距 = 2.f
	fcyVec2 tStartPos(2.f, m_MaxAscender + m_YOffset);
	int tTextCount = m_CurDisplayCount;
	for(size_t i = 0; i<m_DrawText.size(); ++i)
	{
		if(tTextCount == 0)
			break;

		int tCountToDraw = m_DrawText[i].length();

		if(tTextCount < tCountToDraw)
		{
			tCountToDraw = tTextCount;
			tTextCount = 0;
		}
		else
			tTextCount -= tCountToDraw;

		// 绘制一段文本, 边距 = 2.f
		tStartPos = drawText(pGraph->GetGraphics(), m_DrawText[i], tCountToDraw, tProvider, tBlendColor, tStartPos);

		// 执行指令
		if(i != m_DrawText.size() - 1)
		{
			DrawCmd& tCmd = m_DrawCmd[i];
			switch(tCmd.Type)
			{
			case DrawCmd::TYPE_COLOR:
				tBlendColor = *((fcyColor*)&tCmd.ColorData);
				break;
			case DrawCmd::TYPE_FONT:
				tProvider = tCmd.FontData;
				break;
			}
		}
	}

	fuiControl::Render(pGraph);
}
