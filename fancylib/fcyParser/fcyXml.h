//////////////////////////////////////////////////////////////////////////
/// @file  fcyXml.h
/// @brief fcyXml解析器
/// @note  仅能支持一般格式的Xml
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyLexicalParser.h"

#include <unordered_map>
#include <string>
#include <vector>

/// @addtogroup fancy库解析辅助
/// @{

//////////////////////////////////////////////////////////////////////////
/// @brief XML节点
//////////////////////////////////////////////////////////////////////////
class fcyXmlNode
{
	friend class fcyXml;
public:
	class AttributeIterator
	{
		friend class fcyXmlNode;
	private:
		std::unordered_map<std::wstring, std::wstring>::iterator i;
	public:
		fcStrW GetName()const
		{
			return i->first.c_str();
		}
		fcStrW GetContent()const
		{
			return i->second.c_str();
		}
		void SetContent(fcStrW Content)
		{
			i->second = Content;
		}
	public:
		AttributeIterator& operator=(const AttributeIterator& Right)
		{
			i = Right.i;
		}
		fBool operator==(const AttributeIterator& Right)const
		{
			return (i == Right.i);
		}
		fBool operator!=(const AttributeIterator& Right)const
		{
			return (i != Right.i);
		}
		AttributeIterator& operator--()
		{
			i--;
			return *this;
		}
		AttributeIterator& operator++()
		{
			i++;
			return *this;
		}
	protected:
		AttributeIterator(const std::unordered_map<std::wstring, std::wstring>::iterator& _i)
			: i(_i) {}
	public:
		AttributeIterator() {}
		AttributeIterator(const AttributeIterator& Org)
			: i(Org.i) {}
	};
private:
	std::wstring m_Name;                                               ///< @brief 名字
	std::wstring m_Content;                                            ///< @brief 节点数据
	std::vector<fcyXmlNode> m_Nodes;                                   ///< @brief 节点列表
	std::unordered_map<std::wstring, std::wstring> m_Atti;             ///< @brief 属性列表
	std::unordered_map<std::wstring, std::vector<fcyXmlNode*>> m_Dict; ///< @brief 节点缓存
protected:
	/// @brief      写到字符串
	/// @param[out] pOut        输出的字符串
	/// @param[in]  Indentation 缩进
	void writeToStr(std::wstring& pOut, fuInt Indentation);
public:
	/// @brief 拷贝
	fcyXmlNode& operator=(const fcyXmlNode& Org);
public:
	/// @brief 获得节点键名
	fcStrW GetName()const;

	/// @brief 设置节点键名
	void SetName(fcStrW Name);

	/// @brief 获得节点文本
	fcStrW GetContent()const;

	/// @brief 设置节点文本
	void SetContent(fcStrW Context);

	/// @brief 获得子节点个数
	fuInt GetNodeCount()const;

	/// @brief 获得同名子节点个数
	fuInt GetNodeCount(fcStrW NodeName);
	
	/// @brief     获得子节点
	/// @param[in] Index 子节点索引
	/// @return    如果索引越界，则返回NULL
	fcyXmlNode* GetNode(fuInt Index);

	/// @brief     获得子节点
	/// @param[in] Name  节点键名
	/// @param[in] Index 相同键名列表中的索引
	/// @return    如果索引越界或者不存在键名，则返回NULL
	fcyXmlNode* GetNodeByName(fcStrW Name, fuInt Index);

	/// @brief     追加子节点
	/// @note      将自动管理追加的子节点
	/// @param[in] pNode 新节点的指针
	void AppendNode(const fcyXmlNode& pNode);

	/// @brief     移除子节点
	/// @param[in] Index 子节点的索引
	/// @return    返回操作是否成功
	fResult RemoveNode(fuInt Index);

	/// @brief 清空子节点并释放内存
	void ClearNode();

	/// @brief     获得属性
	/// @param[in] Name 属性名
	/// @return    属性值
	fcStrW GetAttribute(fcStrW Name)const;

	/// @brief     获得属性
	/// @param[in] Name  属性名
	/// @param[in] Value 属性值
	void SetAttribute(fcStrW Name, fcStrW Value);

	/// @brief     是否有属性
	/// @param[in] Name 属性名
	/// @return    返回属性是否存在
	fBool HasAttribute(fcStrW Name)const;

	/// @brief 返回第一个属性
	AttributeIterator GetFirstAttribute();

	/// @brief 返回最后一个属性
	AttributeIterator GetLastAttribute();

	/// @brief 移除属性
	fBool RemoveAttribute(fcStrW Name);
	
	/// @brief 移除属性
	AttributeIterator RemoveAttribute(AttributeIterator Iter);
public:
	/// @brief 构造函数
	fcyXmlNode();
	/// @brief 拷贝构造函数
	fcyXmlNode(const fcyXmlNode& Org);
	~fcyXmlNode();
};

//////////////////////////////////////////////////////////////////////////
/// @brief XML解析类
/// @note  本解析类只会解析第一个子节点，不会对XML合法性进行检查。
//////////////////////////////////////////////////////////////////////////
class fcyXml
{
private:
	fcyXmlNode m_pRoot;    ///< @brief 根节点
private: // 预处理
	fBool checkUTF8(fcyStream* pStream);
	fBool checkUTF16LE(fcyStream* pStream);
	std::wstring preprocessXml(fcyStream* pStream);                    ///< @brief 预处理并输出宽字符数据
private: // 解析
	fBool ignoreComment(fcyLexicalReader& tReader);                    ///< @brief 忽略注释
	fBool ignorePreprocess(fcyLexicalReader& tReader);                 ///< @brief 忽略预处理指令
	fBool tryReadCDATA(fcyLexicalReader& tReader, std::wstring& tOut); ///< @brief 试图读取CDATA
	fCharW praseEscape(fcyLexicalReader& tReader);                     ///< @brief 解析转义符
	std::wstring readName(fcyLexicalReader& tReader);                  ///< @brief 读取键名
	std::wstring readString(fcyLexicalReader& tReader);                ///< @brief 读取字符串
	void readAttribute(fcyLexicalReader& tReader, fcyXmlNode* pNode);  ///< @brief 读取属性
	void readNodes(fcyLexicalReader& tReader, fcyXmlNode* pNode);      ///< @brief 读取节点
	fcyXmlNode parserNode(fcyLexicalReader& tReader);                 ///< @brief 解析节点
protected:
	/// @brief 禁止拷贝构造
	fcyXml(const fcyXml& Org) {}
	/// @brief 禁止拷贝构造
	fcyXml& operator=(const fcyXml& Org) {}
public:
	fcyXmlNode* GetRoot();                 ///< @brief 获得根节点
	void SetRoot(const fcyXmlNode& pNode); ///< @brief 设置根节点

	void WriteToStr(std::wstring& pOut); ///< @brief 写到字符串
	void WriteToStream(fcyStream* pOut); ///< @brief 写到流
public:
	fcyXml();                            ///< @brief 构造空的XML
	fcyXml(const std::wstring& Str);     ///< @brief 从字符串构造XML
	fcyXml(fcyStream* pStream);          ///< @brief 从流构造Xml
	~fcyXml();
};

/// @}