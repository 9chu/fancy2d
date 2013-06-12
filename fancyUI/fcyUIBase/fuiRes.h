////////////////////////////////////////////////////////////////////////////////
/// @file  fuiRes.h
/// @brief fancyUI 资源
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyIO/fcyStream.h>
#include <fcyMisc/fcyHash.h>

#include <f2d.h>

#include "fuiProperty.h"

class fuiGraphics;

////////////////////////////////////////////////////////////////////////////////
// 扩充标准库HASH函数
////////////////////////////////////////////////////////////////////////////////
namespace std
{
	template<> 
	struct hash<fcyVec2>
	{
		inline size_t operator()(const fcyVec2& v) const
		{
			return fcyHash::SuperFastHash(v);
		}
	};

	template<typename T1, typename T2>
	struct hash<std::pair<T1,T2>>
	{
		inline size_t operator()(const std::pair<T1,T2>& v) const
		{
			std::hash<T1> hasher;
			fuInt tValue = hasher(v.first);
			fcyHash::HashCombine(tValue, v.second);
			return tValue;
		}
	};
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 资源加载器
////////////////////////////////////////////////////////////////////////////////
struct fuiResProvider
{
	/// @brief 请求渲染器
	virtual f2dRenderer* GetRenderer()=0;

	/// @brief 请求纹理
	/// @note  返回弱引用
	virtual f2dTexture2D* QueryTexture(const std::wstring& ResSrc)=0;
	
	/// @brief 请求字体
	/// @note  返回弱引用
	virtual f2dFontProvider* QueryFont(const std::wstring& ResSrc, const fcyVec2& Size)=0;

	/// @brief 请求纹理字体
	virtual f2dFontProvider* QuerySpriteFont(const std::wstring& TexSrc, const std::wstring& DefineSrc)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 资源加载器实现
////////////////////////////////////////////////////////////////////////////////
class fuiResProviderImpl :
	public fuiResProvider
{
protected:
	f2dFileSys* m_pFileSys;
	f2dRenderer* m_pRenderer;

	std::unordered_map<std::wstring, fcyRefPointer<f2dTexture2D>> m_TexCache;
	std::unordered_map<std::pair<std::wstring, fcyVec2>, fcyRefPointer<f2dFontProvider>> m_FontCache;
	std::unordered_map<std::pair<std::wstring, std::wstring>, fcyRefPointer<f2dFontProvider>> m_SpriteFontCache;
public:
	f2dRenderer* GetRenderer() { return m_pRenderer; }
	f2dTexture2D* QueryTexture(const std::wstring& ResSrc);
	f2dFontProvider* QueryFont(const std::wstring& ResSrc, const fcyVec2& Size);
	f2dFontProvider* QuerySpriteFont(const std::wstring& TexSrc, const std::wstring& DefineSrc);
public:
	fuiResProviderImpl();
	fuiResProviderImpl(f2dFileSys* pFileSys, f2dRenderer* pRenderer);
	fuiResProviderImpl(const fuiResProviderImpl& Org);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 资源基类
////////////////////////////////////////////////////////////////////////////////
class fuiRes :
	public fcyRefObjImpl<fcyRefObj>,
	public fuiPropertySet
{
public:
	enum RESTYPE
	{
		RESTYPE_USER,
		RESTYPE_SPRITE,
		RESTYPE_BORDERSPRITE,
		RESTYPE_FONT
	};
private:
	RESTYPE m_Type;
	std::wstring m_ResName;

	fuiPropertyAccessor<RESTYPE> m_Type_Accessor;
	fuiPropertyAccessor<std::wstring> m_ResName_Accessor;
public:
	RESTYPE GetResType() { return m_Type; }
	const std::wstring& GetResName() { return m_ResName; }
public:
	/// @brief 构造资源
	virtual void ConstructRes(fuiResProvider* pProvider) {}
public:
	fuiRes(RESTYPE Type, const std::wstring& Name);
protected:
	virtual ~fuiRes() {}
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 精灵
////////////////////////////////////////////////////////////////////////////////
class fuiSprite :
	public fuiRes
{
protected:
	std::wstring m_TexPath;
	fcyRect m_Rect;
	fcyVec2 m_HotPos;
	fBool m_bSetHotPos;

	fuiPropertyAccessor<std::wstring> m_TexPath_Accessor;
	fuiPropertyAccessor<fcyRect> m_Rect_Accessor;
	fuiPropertyAccessor<fcyVec2> m_HotPos_Accessor;
protected:
	fcyRefPointer<f2dSprite> m_pSprite;
public:
	f2dSprite* GetSprite() { return m_pSprite; }
public:
	void ConstructRes(fuiResProvider* pProvider);
public:
	fuiSprite(const std::wstring& Name);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 边框精灵
////////////////////////////////////////////////////////////////////////////////
class fuiBorderSprite :
	public fuiRes
{
protected:
	std::wstring m_TexPath;
	fcyRect m_Rect;
	fcyRect m_Margin;

	fuiPropertyAccessor<std::wstring> m_TexPath_Accessor;
	fuiPropertyAccessor<fcyRect> m_Rect_Accessor;
	fuiPropertyAccessor<fcyRect> m_Margin_Accessor;
protected:
	fcyRefPointer<f2dSprite> m_pBorderSprite;  ///< @brief 边框图
	fcyRect m_Sprites[3][3];                   ///< @brief 九宫格位置
public:
	f2dSprite* GetSprite() { return m_pBorderSprite; }

	void Draw(fuiGraphics* pGraph, const fcyRect& Pos, const fcyColor& Color=0xFF000000);
public:
	void ConstructRes(fuiResProvider* pProvider);
public:
	fuiBorderSprite(const std::wstring& Name);
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 字体
////////////////////////////////////////////////////////////////////////////////
class fuiFont :
	public fuiRes
{
protected:
	std::wstring m_FontSrc;
	std::wstring m_TexSrc;
	std::wstring m_Define;
	fcyVec2 m_FontSize;

	fuiPropertyAccessor<std::wstring> m_FontSrc_Accessor;
	fuiPropertyAccessor<std::wstring> m_TexSrc_Accessor;
	fuiPropertyAccessor<std::wstring> m_Define_Accessor;
	fuiPropertyAccessor<fcyVec2> m_FontSize_Accessor;
protected:
	fcyRefPointer<f2dFontProvider> m_pProvider;
	fcyRefPointer<f2dFontRenderer> m_pRenderer;
public:
	f2dFontProvider* GetFontProvider() { return m_pProvider; }
	f2dFontRenderer* GetFontRenderer() { return m_pRenderer; }
public:
	void ConstructRes(fuiResProvider* pProvider);
public:
	fuiFont(const std::wstring& Name);
};
