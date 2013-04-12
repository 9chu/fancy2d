#pragma once

#include "fcyParser/fcyXml.h"

#include "f2d.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI上下文资源加载器
////////////////////////////////////////////////////////////////////////////////
struct fuiContextResLoader
{
	/// @brief  由路径返回纹理
	/// @return 错误返回NULL
	virtual f2dTexture* GetTexture(fcStrW Path)=0;

	/// @brief  由路径返回字体
	/// @return 错误返回NULL
	virtual f2dFontRenderer* GetFont(fcStrW Path, fuInt Size)=0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI上下文
////////////////////////////////////////////////////////////////////////////////
class fuiContext :
	public fcyRefObjImpl<fcyRefObj>
{
private:
	/// @brief 默认资源加载器
	class DefaultResLoader :
		public fuiContextResLoader
	{
	private:
		f2dRenderer* m_pRenderer;

		std::unordered_map<std::wstring, f2dTexture*> m_TexCache;      ///< @brief 纹理缓存
		std::unordered_map<std::wstring, fcyStream*> m_pFontFileCache; ///< @brief 字体文件缓存
		std::unordered_map<std::wstring, std::unordered_map<fuInt, f2dFontRenderer*> > m_FontCache; ///< @brief 字体缓存
	private:
		fcyStream* openStream(fcStrW Path);
	public:
		f2dTexture* GetTexture(fcStrW Path);
		f2dFontRenderer* GetFont(fcStrW Path, fuInt Size);
	public:
		DefaultResLoader(f2dRenderer* pRenderer);
		~DefaultResLoader();
	};
private:
	// === 文件源 ===
	DefaultResLoader m_DefaultLoader;
	fuiContextResLoader* m_ResLoader;

	// === 引擎对象 ===
	f2dRenderer* m_pRenderer; ///< @brief  用以资源创建

	// === 缓存 ===
	std::unordered_map<std::wstring, f2dSprite*> m_SpriteCache;     ///< @brief 精灵缓存
	std::unordered_map<std::wstring, f2dFontRenderer*> m_FontCache; ///< @brief 文字缓存

	// === 样式信息 ===
	std::unordered_map<std::wstring, std::unordered_map<std::wstring, fcyXmlNode> > m_Style;
public:
	/// @brief 返回资源加载器
	fuiContextResLoader* GetResLoader();
	/// @brief     设置资源加载器
	/// @param[in] pLoader 加载器
	void SetResLoader(fuiContextResLoader* pLoader);
	/// @brief   清空精灵
	/// @warning UI对象仅保存对精灵的引用。请在确保安全的前提下使用本函数。
	void ClearSprite();
	/// @brief   清空字体样式
	/// @warning UI对象仅保存对字体样式的引用。请在确保安全的前提下使用本函数。
	void ClearFontStyle();
	/// @brief 清空样式
	void ClearStyle();
	/// @brief     从文件加载精灵定义
	/// @exception fcyException fancy异常
	/// @param[in] pFile 包含定义的XML文件
	void LoadSprite(fcyStream* pFile);
	/// @brief     从XML节点加载定义
	/// @exception fcyException fancy异常
	/// @param[in] pNode 包含定义的XML节点
	void LoadSprite(fcyXmlNode* pNode);
	/// @brief     从文件加载UI定义
	/// @exception fcyException fancy异常
	/// @param[in] pFile 包含定义的XML文件
	void LoadStyle(fcyStream* pFile);
	/// @brief     从XML节点加载UI定义
	/// @exception fcyException fancy异常
	/// @param[in] pNode 包含定义的XML节点
	void LoadStyle(fcyXmlNode* pNode);
	/// @brief     从文件加载字体样式定义
	/// @exception fcyException fancy异常
	/// @param[in] pFile 包含定义的XML文件
	void LoadFontStyle(fcyStream* pFile);
	/// @brief     从XML节点加载字体样式定义
	/// @exception fcyException fancy异常
	/// @param[in] pNode 包含定义的XML节点
	void LoadFontStyle(fcyXmlNode* pNode);
	/// @brief     获得精灵
	/// @param[in] SpriteName 精灵名称
	f2dSprite* GetSprite(fcStrW SpriteName);
	/// @brief     获得样式
	/// @param[in] ControlType 控件类型
	/// @param[in] StyleName   样式名称
	fcyXmlNode* GetStyle(fcStrW ControlType, fcStrW StyleName);
	/// @brief     获得字体
	/// @param[in] FontName 字体名称
	f2dFontRenderer* GetFont(fcStrW FontName);
public:
	/// @brief     构造函数
	/// @param[in] f2d渲染器
	fuiContext(f2dRenderer* pRenderer);
protected:
	/// @brief 禁止直接析构
	~fuiContext();
};
