////////////////////////////////////////////////////////////////////////////////
/// @file  f2dRendererImpl.h
/// @brief fancy2D渲染器接口实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dRenderDeviceImpl.h"

#include <unordered_map>

#include <ft2build.h> 
#include FT_FREETYPE_H

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D Sprite实现
////////////////////////////////////////////////////////////////////////////////
class f2dSpriteImpl : 
	public fcyRefObjImpl<f2dSprite>
{
	friend class f2dRendererImpl;
private:
	f2dTexture* m_pTex;
	F2DSPRITEFLIP m_FlipType;
	fcyRect m_Org;
	fcyVec2 m_HotSpot;
	fcyColor m_Color[4];
	float m_ZValue;

	fcyRect m_UV;
	fcyRect m_SpriteRect;
private:
	void recalcu();
public: // 接口实现
	f2dTexture* GetTexture();
	fResult SetTexture(f2dTexture* pTex);
	fcyRect GetTexRect();
	fResult SetTexRect(fcyRect Rect);
	fcyVec2 GetHotSpot();
	fResult SetHotSpot(fcyVec2 Point);
	F2DSPRITEFLIP GetFlipType();
	fResult SetFlipType(F2DSPRITEFLIP Type);
	fFloat GetZ();
	fResult SetZ(fFloat Value);
	fcyColor GetColor(fuInt Index);
	fResult SetColor(fcyColor Color);
	fResult SetColor(fuInt Index, fcyColor Color);
	fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest);
	fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, const fcyRect& SubTex);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex);
protected:
	f2dSpriteImpl(f2dTexture* pTex);
	f2dSpriteImpl(f2dTexture* pTex, fcyRect SrcRect);
	f2dSpriteImpl(f2dTexture* pTex, fcyRect SrcRect, fcyVec2 HotSpot);
	~f2dSpriteImpl();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D 文字渲染器实现
////////////////////////////////////////////////////////////////////////////////
class f2dFontRendererImpl :
	public fcyRefObjImpl<f2dFontRenderer>,
	public f2dRenderDeviceEventListener
{
	friend class f2dRendererImpl;
private:
	// 字体缓冲
	struct TextureCache
	{
		// 字体属性
		fCharW fontChar;    // 字符
		fuInt fontWidth;    // 文字图像有效宽度
		fuInt fontHeight;   // 文字图像有效高度
		int fontLeft;       // 文字左端距离绘制起点的距离
		int fontTop;        // 文字顶端距离基线的高度
		fcyVec2 fontAdvance;// 绘制点推进量

		// 渲染属性
		fcyRect uv;

		// 链表域
		TextureCache* pPrev;
		TextureCache* pNext;
	};

	// 高级字体风格参数
	struct AdvancedTextParam
	{
		union
		{
			int IntData;
			float FloatData;
		};
	};

	// 高级字体风格
	struct AdvanceTextFormat
	{
		F2DSPRITEFLIP FlipType;       // 翻转
		float Bias;                   // 倾斜
		const fcyVec2& StartPos;      // 起始位置
		fcyVec2 Pos;                  // 笔触位置
		fcyVec2* PosCache;            // 缓存
		fuInt CacheCount;             // 缓存数量
		float LineHeight;             // 行高
		f2dGraphics2DVertex Verts[4]; // 顶点
	};
private:
	f2dRenderDevice* m_pDev;
	f2dStream* m_pStream;
	std::wstring m_Using;     // 用于恢复缓存

	// --- 渲染选项 ---
	F2DSPRITEFLIP m_FlipType;
	fcyColor m_BlendColor[4];
	fFloat m_ZValue;

	// --- freetype组件 ---
	FT_Open_Args m_Args;
	FT_StreamRec m_Stream;
	FT_Library m_FontLib;
	FT_Face m_Face;

	// --- 纹理缓冲区 ---
	fuInt m_CacheXMax;              // 缓冲横向数量
	fuInt m_CacheYMax;              // 缓冲纵向数量
	fuInt m_CacheCurWidth;          // 缓冲单个字体宽度
	fuInt m_CacheCurHeight;         // 缓冲单个字体高度
	TextureCache* m_Cache;          // 缓冲区
	f2dTextureDynamic* m_CacheTex;  // 缓冲区纹理

	// --- 最近使用 ---
	std::unordered_map<fCharW, TextureCache*> m_Dict; // 查询用
	TextureCache* m_UsedNode;  // 使用中节点，保证为循环链表
	TextureCache* m_FreeNode;  // 空闲节点，单向链表
private:
	static unsigned long streamRead(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count);
	static void streamClose(FT_Stream stream);
private:
	void addUsedNode(TextureCache* p);      // 加入最多使用节点
	void removeFreeNode(TextureCache* p);   // 移除空闲节点p的连接
	void removeUsedNode(TextureCache* p);   // 移除使用中节点p的连接
	float widthSizeToPixel(int Size);       // 宽度单位到像素
	float heightSizeToPixel(int Size);      // 高度单位到像素
	TextureCache* getChar(fCharW Char);     // 获得字体
	bool makeCache(fuInt XCount, fuInt YCount);                        // 创建缓冲区，存放XCount * YCount个文字
	bool renderCache(TextureCache* pCache, fCharW Char, bool Discard); // 在缓冲区的[x][y]位置绘制字体Char
private:
	fBool parseAdvancedCommand(fcStrW Text, fuInt Count, fuInt& Pos, fCharW Cmd[]);
	fBool parseAdvancedParam(fcStrW Text, fuInt Count, fuInt& Pos, AdvancedTextParam Param[], fuInt& ParamCount);
	// 解析高级文字并设置属性
	// 返回可打印字符
	fCharW parseAdvancedText(fcStrW Text, fuInt Count, fuInt& Pos, AdvanceTextFormat& Fmt);
public: // 事件监听
	void OnRenderDeviceLost();
	void OnRenderDeviceReset();
public: // 接口实现
	fuInt GetFaceCount();
	fFloat GetLineHeight();
	f2dTexture* GetCache();
	void Cache(fcStrW Text);
	fFloat GetZ();
	void SetZ(fFloat ZValue);
	fcyColor GetColor(fuInt Index);
	void SetColor(const fcyColor& Color);
	void SetColor(fuInt Index, const fcyColor& Color);
	F2DSPRITEFLIP GetFlipType();
	void SetFlipType(F2DSPRITEFLIP Type);
	fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos);
	fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut);
	fResult DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut);
	fResult DrawTextInRect(f2dGraphics2D* pGraph, fcStrW Text, const fcyRect& DestRect, fuInt* pCountRendered);
	fResult DrawTextInRect(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyRect& DestRect, fuInt* pCountRendered);
	fResult DrawAdvancedText(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosCache, fuInt PosCacheCount);
	fResult DrawAdvancedTextInRect(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyRect& DestRect, fcyVec2* PosCache, fuInt PosCacheCount, fuInt* pCountRendered);
protected:
	f2dFontRendererImpl(f2dRenderDevice* pDev, fcyStream* pStream, fuInt FontWidth, fuInt FontHeight, fuInt FaceIndex);
	~f2dFontRendererImpl();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D几何体渲染器实现
////////////////////////////////////////////////////////////////////////////////
class f2dGeometryRendererImpl :
	public fcyRefObjImpl<f2dGeometryRenderer>
{
	friend class f2dRendererImpl;
private:
	fcyColor m_PenColor[3];
	fFloat m_PenSize;
	fFloat m_ZValue;

	std::vector<f2dGraphics2DVertex> m_CacheVerts;
	std::vector<fuShort> m_CacheIndex;
public:
	fFloat GetZ();
	void SetZ(fFloat ZValue);
	fcyColor GetPenColor(fuInt Index);
	void SetPenColor(fuInt Index, const fcyColor& Color);
	fFloat GetPenSize();
	void SetPenSize(fFloat Size);

	fResult DrawLine(f2dGraphics2D* pGraph, const fcyVec2& Begin, const fcyVec2& End);
	fResult DrawRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect);
	fResult DrawCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, fuInt Divide);

	fResult FillRectangle(f2dGraphics2D* pGraph, const fcyRect& Rect, const fcyColor* pColorArr);
	fResult FillCircle(f2dGraphics2D* pGraph, const fcyVec2& Center, float R, const fcyColor& CenterColor, const fcyColor& BoundColor, fuInt Divide);
protected:
	f2dGeometryRendererImpl();
	~f2dGeometryRendererImpl();
};

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D渲染器实现
////////////////////////////////////////////////////////////////////////////////
class f2dRendererImpl :
	public fcyRefObjImpl<f2dRenderer>
{
private:
	f2dEngineImpl* m_pEngine;

	f2dRenderDeviceImpl* m_pDev;
public: // 接口实现
	f2dRenderDevice* GetDevice();

	fResult CreateSprite2D(f2dTexture* pTex, f2dSprite** pOut);
	fResult CreateSprite2D(f2dTexture* pTex, const fcyRect& Org, f2dSprite** pOut);
	fResult CreateSprite2D(f2dTexture* pTex, const fcyRect& Org, const fcyVec2& HotSopt, f2dSprite** pOut);

	fResult CreateFontRenderer(f2dStream* pStream, fuInt FontWidth, fuInt FontHeight, fuInt FaceIndex, f2dFontRenderer** pRenderer);

	fResult CreateGeometryRenderer(f2dGeometryRenderer** pOut);
public:
	f2dRendererImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel);
	~f2dRendererImpl();
};
