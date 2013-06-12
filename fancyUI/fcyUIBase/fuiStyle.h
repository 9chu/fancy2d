////////////////////////////////////////////////////////////////////////////////
/// @file  fuiStyle.h
/// @brief fancyUI 控件样式
/// @note  负责控件渲染资源的管理
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fuiGraphics.h"
#include "fuiRes.h"

#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
/// @brief 样式对象
////////////////////////////////////////////////////////////////////////////////
class fuiStyle :
	public fcyRefObjImpl<fcyRefObj>
{
protected:
	std::unordered_map<std::wstring, fcyRefPointer<fuiRes>> m_ResList;
public:
	/// @brief 注册资源
	void RegisterRes(const std::wstring Name, fuiRes* pRes);
	
	/// @brief 移除资源
	void UnregisterRes(const std::wstring Name);

	/// @brief 查询资源
	/// @note  返回弱引用
	fuiRes* QueryRes(const std::wstring Name);

	/// @brief 从文件构造
	void LoadResFromFile(f2dStream* pStream, fuiResProvider* pProvider);
public:
	fuiStyle();
	~fuiStyle();
};
