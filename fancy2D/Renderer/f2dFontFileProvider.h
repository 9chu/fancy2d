////////////////////////////////////////////////////////////////////////////////
/// @file  f2dFontFileProvider.h
/// @brief fancy2D 字体提供者
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include "../f2dRenderer.h"

#include <unordered_map>

#include <ft2build.h> 
#include FT_FREETYPE_H

////////////////////////////////////////////////////////////////////////////////
/// @brief 字体提供者实现
////////////////////////////////////////////////////////////////////////////////
class f2dFontFileProvider :
	public fcyRefObjImpl<f2dFontProvider>
{
	friend class f2dRendererImpl;
protected:
	// 字体缓冲属性
	struct FontCacheInfo
	{
		// 字体属性
		fCharW Character;
		fcyRect UV;

		// 链表域
		FontCacheInfo* pPrev;
		FontCacheInfo* pNext;
	};
protected:
	f2dFontFileProvider();
	~f2dFontFileProvider();
};
