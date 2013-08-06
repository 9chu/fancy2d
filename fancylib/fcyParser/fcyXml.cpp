#include "fcyXml.h"

#include "../fcyMisc/fcyStringHelper.h"
#include "../fcyIO/fcyBinaryHelper.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fcyXmlNode::fcyXmlNode()
{
}

fcyXmlNode::fcyXmlNode(const fcyXmlNode& Org)
{
	m_Name = Org.m_Name;
	m_Content = Org.m_Content;
	m_Atti = Org.m_Atti;

	m_Nodes.reserve(Org.m_Nodes.size());
	for(fuInt i = 0; i<Org.m_Nodes.size(); ++i)
	{
		AppendNode(Org.m_Nodes[i]);
	}
}

fcyXmlNode::~fcyXmlNode()
{
	ClearNode();
}

void fcyXmlNode::writeToStr(wstring& pOut, fuInt Indentation)
{
	// 缩进
	for(fuInt i = 0; i<Indentation; ++i)
	{
		pOut += L"\t";
	}

	// 写出标签头
	pOut += L"<" + m_Name;
	
	// 写属性
	if(m_Atti.size())
	{
		pOut += L' ';

		unordered_map<wstring, wstring>::iterator i = m_Atti.begin();
		while(i != m_Atti.end())
		{
			pOut += i->first + L"=\"";
			
			// 写出属性值并转义
			for(fuInt k = 0; k<i->second.size(); k++)
			{
				switch(i->second[k])
				{
				case L'<':
					pOut += L"&lt;";
					break;
				case L'>':
					pOut += L"&gt;";
					break;
				case L'&':
					pOut += L"&amp;";
					break;
				case L'\'':
					pOut += L"&apos;";
					break;
				case L'"':
					pOut += L"&quot;";
					break;
				default:
					pOut += i->second[k];
					break;
				}
			}

			pOut += L"\" ";
			++i;
		}
	}

	// 结尾？
	if(m_Content.size()==0 && m_Nodes.size()==0)
	{
		pOut += L"/>\n";
		return;
	}
	else
	{
		pOut += L">";
	}

	// 写出子节点
	if(m_Nodes.size())
	{
		// 换行
		pOut += L"\n";

		unordered_map<wstring, vector<fcyXmlNode*>>::iterator i = m_Dict.begin();

		while(i != m_Dict.end())
		{
			vector<fcyXmlNode*>::iterator j = i->second.begin();
			while(j != i->second.end())
			{
				(*j)->writeToStr(pOut, Indentation + 1);
				j++;
			}
			++i;
		}
	}
	else if(m_Content.size())
	{
		// 写出Content
		for(fuInt i = 0; i<m_Content.size(); ++i)
		{
			switch(m_Content[i])
			{
			case L'<':
				pOut += L"&lt;";
				break;
			case L'>':
				pOut += L"&gt;";
				break;
			case L'&':
				pOut += L"&amp;";
				break;
			case L'\'':
				pOut += L"&apos;";
				break;
			case L'"':
				pOut += L"&quot;";
				break;
			default:
				pOut += m_Content[i];
				break;
			}
		}
	}

	// 闭合标签
	pOut += L"</" + m_Name + L">\n";
}

fcyXmlNode& fcyXmlNode::operator=(const fcyXmlNode& Org)
{
	ClearNode();

	m_Name = Org.m_Name;
	m_Content = Org.m_Content;
	m_Atti = Org.m_Atti;

	m_Nodes.reserve(Org.m_Nodes.size());
	for(fuInt i = 0; i<Org.m_Nodes.size(); ++i)
	{
		AppendNode(Org.m_Nodes[i]);
	}

	return *this;
}

fcStrW fcyXmlNode::GetName()const
{
	return m_Name.c_str();
}

void fcyXmlNode::SetName(fcStrW Name)
{
	m_Name = Name;
}

fcStrW fcyXmlNode::GetContent()const
{
	return m_Content.c_str();
}

void fcyXmlNode::SetContent(fcStrW Context)
{
	m_Content = Context;
}

fuInt fcyXmlNode::GetNodeCount()const
{
	return m_Nodes.size();
}

fuInt fcyXmlNode::GetNodeCount(fcStrW NodeName)const
{
	unordered_map<wstring, vector<fcyXmlNode*>>::const_iterator i = m_Dict.find(NodeName);
	if(i == m_Dict.end())
		return 0;

	return i->second.size();
}

fcyXmlNode* fcyXmlNode::GetNode(fuInt Index)
{
	if(Index>=GetNodeCount())
		return NULL;

	return &(m_Nodes[Index]);
}

const fcyXmlNode* fcyXmlNode::GetNode(fuInt Index)const
{
	if(Index>=GetNodeCount())
		return NULL;

	return &(m_Nodes[Index]);
}

fcyXmlNode* fcyXmlNode::GetNodeByName(fcStrW Name, fuInt Index)
{
	unordered_map<wstring, vector<fcyXmlNode*>>::iterator i = m_Dict.find(Name);
	if(i == m_Dict.end())
		return NULL;

	if(Index >= i->second.size())
		return NULL;
	else
		return i->second[Index];
}

const fcyXmlNode* fcyXmlNode::GetNodeByName(fcStrW Name, fuInt Index)const
{
	unordered_map<wstring, vector<fcyXmlNode*>>::const_iterator i = m_Dict.find(Name);
	if(i == m_Dict.end())
		return NULL;

	if(Index >= i->second.size())
		return NULL;
	else
		return i->second[Index];
}

void fcyXmlNode::AppendNode(const fcyXmlNode& pNode)
{
	m_Nodes.push_back(pNode);

	m_Dict[pNode.GetName()].push_back(&m_Nodes.back());
}

fResult fcyXmlNode::RemoveNode(fuInt Index)
{
	if(Index>=GetNodeCount())
		return FCYERR_INVAILDPARAM;

	vector<fcyXmlNode>::iterator i = m_Nodes.begin()+Index;

	unordered_map<wstring, std::vector<fcyXmlNode*>>::iterator j = 
		m_Dict.find((*i).GetName());

	if(j != m_Dict.end())
	{
		vector<fcyXmlNode*>::iterator k = j->second.begin();
		while(k != j->second.end())
		{
			if(*k == &(*i))
			{
				j->second.erase(k);
				break;
			}
			k++;
		}
		j++;
	}

	m_Nodes.erase(i);

	return FCYERR_OK;
}

void fcyXmlNode::ClearNode()
{
	m_Nodes.clear();
	m_Dict.clear();
}

fcStrW fcyXmlNode::GetAttribute(fcStrW Name)const
{
	unordered_map<std::wstring, std::wstring>::const_iterator i = m_Atti.find(Name);
	if(i == m_Atti.end())
		return NULL;
	else
		return i->second.c_str();
}

void fcyXmlNode::SetAttribute(fcStrW Name, fcStrW Value)
{
	m_Atti[Name] = Value;
}

fBool fcyXmlNode::HasAttribute(fcStrW Name)const
{
	return (m_Atti.find(Name) != m_Atti.end());
}

fcyXmlNode::AttributeIterator fcyXmlNode::GetFirstAttribute()
{
	return AttributeIterator(m_Atti.begin());
}

fcyXmlNode::AttributeIterator fcyXmlNode::GetLastAttribute()
{
	return AttributeIterator(m_Atti.end());
}

fBool fcyXmlNode::RemoveAttribute(fcStrW Name)
{
	unordered_map<std::wstring, std::wstring>::iterator i = m_Atti.find(Name);
	if(i == m_Atti.end())
		return false;
	else
	{
		m_Atti.erase(i);
		return true;
	}
}

fcyXmlNode::AttributeIterator fcyXmlNode::RemoveAttribute(fcyXmlNode::AttributeIterator Iter)
{
	return m_Atti.erase(Iter.i);
}

////////////////////////////////////////////////////////////////////////////////

fcyXml::fcyXml()
{
}

fcyXml::fcyXml(const wstring& Str)
{
	fcyLexicalReader tReader(Str);
	
	// 去预处理
	while(ignorePreprocess(tReader));
	// 去注释
	while(ignoreComment(tReader));

	m_pRoot = parserNode(tReader);
}

fcyXml::fcyXml(fcyStream* pStream)
{
	fcyLexicalReader tReader(preprocessXml(pStream));

	// 去预处理
	while(ignorePreprocess(tReader));
	// 去注释
	while(ignoreComment(tReader));

	m_pRoot = parserNode(tReader);
}

fcyXml::~fcyXml(void)
{
}

fBool fcyXml::checkUTF8(fcyStream* pStream)
{
	fLen tPos = pStream->GetPosition();

	// 尝试读取BOM头
	fByte tBomCache[3];
	if(FCYOK(pStream->ReadBytes(tBomCache, 3, NULL)))
	{
		fByte tUTF8[3] = { 0xEF, 0xBB, 0xBF };
		if(memcmp(tBomCache, tUTF8, 3)==0)
			return true;
		else
			pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);
	}
	else
			pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);

	return false;
}

fBool fcyXml::checkUTF16LE(fcyStream* pStream)
{
	fLen tPos = pStream->GetPosition();

	// 尝试读取BOM头
	fByte tBomCache[2];
	if(FCYOK(pStream->ReadBytes(tBomCache, 2, NULL)))
	{
		fByte tUTF16LE[2] = { 0xFF, 0xFE };
		if(memcmp(tBomCache, tUTF16LE, 2)==0)
			return true;
		else
			pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);
	}
	else
			pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);

	return false;
}

wstring fcyXml::preprocessXml(fcyStream* pStream)
{
	// 在有BOM头的时候忽略预处理指令
	if(checkUTF16LE(pStream))
	{
		wstring tRet;
		fLen tSize = pStream->GetLength() - pStream->GetPosition();
		tRet.resize((size_t)tSize);
		pStream->ReadBytes((fData)&tRet[0], tSize, NULL);
		return tRet;
	}
	else if(checkUTF8(pStream))
	{
		string tTemp;
		fLen tSize = pStream->GetLength() - pStream->GetPosition();
		tTemp.resize((size_t)tSize);
		pStream->ReadBytes((fData)&tTemp[0], tSize, NULL);
		return fcyStringHelper::MultiByteToWideChar(tTemp, CP_UTF8);
	}
	else
	{
		// 依次读取前面的字符，解析预处理指令
		fInt tCodePage = CP_UTF8;
		fLen tPos = pStream->GetPosition();

		fcyBinaryReader tReader(pStream);

		try
		{
			fChar tFlag[6] = { 0 };
			tReader.ReadChars(tFlag, 5);
			if(strcmp(tFlag, "<?xml")==0) // 匹配<?xml
			{
				unordered_map<string, string> tAttrib;
				string tKey;
				string tValue;

				while(1)
				{
					fChar tChar = tReader.ReadChar();

					if(isspace(tChar))
						continue;
					else if(tChar == '?')
					{
						tChar = tReader.ReadChar();
						if(tChar == '>')
							break;
						else
							throw fcyException("fcyXml::preprocessXml", "expect >");
					}
					else if(tChar == '=')
					{
						if(tKey.empty())
							throw fcyException("fcyXml::preprocessXml", "expect name but found =");
						
						// 读取value
						fBool tMatch = false;
						while(1)
						{
							fChar tChar = tReader.ReadChar();
							
							if(isspace(tChar))
								continue;
							else if(tChar == '"')
							{
								if(tMatch == true)
									break;
								else
									tMatch = true;
							}
							else
							{
								tValue += tChar;
							}
						}
						tAttrib[tKey] = tValue;
						tKey = tValue = "";
					}
					else
					{
						tKey += tChar;
					}
				}

				// 获取信息
				if(tAttrib.find("version")!=tAttrib.end() && atof(tAttrib["version"].c_str()) != 1.0)
					throw fcyException("fcyXml::preprocessXml", "xml version not support");
				if(tAttrib.find("encoding")!=tAttrib.end())
				{
					if(tAttrib["encoding"] == "UTF-8")
						tCodePage = CP_UTF8;
					else if(tAttrib["encoding"] == "gb2312")
						tCodePage = 936;
					else
						throw fcyException("fcyXml::preprocessXml", "unknown encoding");
				}
			}
			else
				throw fcyException("fcyXml::preprocessXml", "expect <?xml");
		}
		catch(...)
		{
			pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);
		}

		// 读取并转换
		string tTemp;
		fLen tSize = pStream->GetLength() - pStream->GetPosition();
		tTemp.resize((size_t)tSize);
		pStream->ReadBytes((fData)&tTemp[0], tSize, NULL);
		return fcyStringHelper::MultiByteToWideChar(tTemp, tCodePage);
	}
}

fBool fcyXml::ignoreComment(fcyLexicalReader& tReader)
{
	if(tReader.TryMatch(L"<!--", true, true))
	{
		while(1)
		{
			fCharW tChar = tReader.ReadChar();

			if(tChar == L'-')
			{
				if(tReader.TryMatch(L"->", false, true))
					return true;
			}
		}
	}
	return false;
}

fBool fcyXml::ignorePreprocess(fcyLexicalReader& tReader)
{
	if(tReader.TryMatch(L"<?xml", true, true))
	{
		unordered_map<wstring, wstring> tAttrib;
		wstring tKey;
		wstring tValue;

		while(1)
		{
			fCharW tChar = tReader.ReadChar();

			if(isspace(tChar))
				continue;
			else if(tChar == L'?')
			{
				tChar = tReader.ReadChar();
				if(tChar == L'>')
					break;
				else
					throw fcyException("fcyXml::ignorePreprocess", "expect >");
			}
			else if(tChar == L'=')
			{
				if(tKey.empty())
					throw fcyException("fcyXml::ignorePreprocess", "expect name but found =");

				// 读取value
				fBool tMatch = false;
				while(1)
				{
					fCharW tChar = tReader.ReadChar();

					if(isspace(tChar))
						continue;
					else if(tChar == L'"')
					{
						if(tMatch == true)
							break;
						else
							tMatch = true;
					}
					else
					{
						tValue += tChar;
					}
				}
				tAttrib[tKey] = tValue;
				tKey = tValue = L"";
			}
			else
			{
				tKey += tChar;
			}
		}
		return true;
	}
	return false;
}

fBool fcyXml::tryReadCDATA(fcyLexicalReader& tReader, std::wstring& tOut)
{
	tOut = L"";
	if(tReader.TryMatch(L"<![CDATA[", false, true))
	{
		while(!tReader.TryMatch(L"]]>", false, true))
		{
			tOut.push_back(tReader.ReadChar());
		}

		return true;
	}
	return false;
}

fCharW fcyXml::praseEscape(fcyLexicalReader& tReader)
{
	if(tReader.TryMatch(L"&#", false, true))
	{
		wstring tNum;
		fCharW tChar;
		while((tChar = tReader.ReadChar())!=L';')
		{
			tNum += tChar;
		}
		return (fCharW)_wtoi(tNum.c_str());
	}
	if(tReader.TryMatch(L"&lt;", false, true))
		return L'<';
	if(tReader.TryMatch(L"&gt;", false, true))
		return L'>';
	if(tReader.TryMatch(L"&amp;", false, true))
		return L'&';
	if(tReader.TryMatch(L"&apos;", false, true))
		return L'\'';
	if(tReader.TryMatch(L"&quot;", false, true))
		return L'\"';
	throw fcyLexicalException("fcyXml::praseEscape", "Invalid escape char.", tReader.GetLine(), tReader.GetRow());
}

wstring fcyXml::readName(fcyLexicalReader& tReader)
{
	wstring tRet;

	fCharW tChar = tReader.PeekChar();
	if(tChar == L'/')  // 不能以'/'开头
		throw fcyLexicalException("fcyXml::readName", "Name can't begin with '/'.", tReader.GetLine(), tReader.GetRow());
	if(tChar == L'_')  // 不能以下划线开头
		throw fcyLexicalException("fcyXml::readName", "Name can't begin with '_'.", tReader.GetLine(), tReader.GetRow());
	if(iswspace(tChar)) // 不能以空白符开头
		throw fcyLexicalException("fcyXml::readName", "Name can't begin with space.", tReader.GetLine(), tReader.GetRow());
	if(iswdigit(tChar)) // 不能以数字开头
		throw fcyLexicalException("fcyXml::readName", "Name can't begin with digit.", tReader.GetLine(), tReader.GetRow());
	
	while(!(iswspace(tChar) || tChar==L'/' || tChar==L'<' || tChar==L'>' || tChar==L'"' || tChar==L'=' || tChar==L'&'))
	{
		tRet += tChar;
		tReader.ReadChar();
		tChar = tReader.PeekChar();
	}

	return tRet;
}

std::wstring fcyXml::readString(fcyLexicalReader& tReader)
{
	wstring tRet;

	// 读取'"'
	tReader.Match(L'"', true);

	while(1)
	{
		fCharW tChar = tReader.PeekChar();

		if(tChar == L'"')
		{
			tReader.ReadChar();
			break;
		}
		else if(tChar == L'&') // 处理escape
		{
			tRet += praseEscape(tReader);
			continue;
		}

		tRet += tChar;
		tChar = tReader.ReadChar();
	}

	return tRet;
}

void fcyXml::readAttribute(fcyLexicalReader& tReader, fcyXmlNode* pNode)
{
	while(!(tReader.TryMatch(L'>', true, false) || tReader.TryMatch(L"/>", true, false)))
	{
		// 读取属性名
		wstring tName = readName(tReader);

		// 读取'='
		tReader.Match(L'=', true);

		// 读取字符串
		wstring tStr = readString(tReader);

		tReader.IgnoreSpace();

		// 加入属性
		pNode->SetAttribute(tName.c_str(), tStr.c_str());
	}
}

void fcyXml::readNodes(fcyLexicalReader& tReader, fcyXmlNode* pNode)
{
	wstring tContent;

	// 去注释
	while(ignoreComment(tReader));

	// 检查分支
	if(tReader.TryMatch(L'<', true, false) && !tReader.TryMatch(L"<![CDATA[", true, false))
	{
		// 作为子节点
		while(!tReader.TryMatch(L"</", true, false))
		{
			// 解析并加入标签
			pNode->AppendNode(parserNode(tReader));

			// 去注释
			while(ignoreComment(tReader));
		}
	}
	else
	{
		// 作为文本节点
		fCharW tChar = tReader.PeekChar();
		while(true)
		{
			fBool tEnd = false;

			// 处理CDATA和注释
			while(tChar == L'<')
			{
				ignoreComment(tReader);

				tChar = tReader.PeekChar();
				
				if(tChar == L'<')
				{
					wstring tData;

					if(tryReadCDATA(tReader, tData))
					{
						tContent += tData;
						tChar = tReader.PeekChar();
					}
					else
					{
						tEnd = true;
						break;
					}
				}
			}

			if(tEnd)
				break;

			// 处理escape
			if(tChar == L'&')
			{
				tContent += praseEscape(tReader);
				tChar = tReader.PeekChar();
				continue;
			}

			tContent += tChar;
			tReader.ReadChar();
			tChar = tReader.PeekChar();
		}

		pNode->SetContent(tContent.c_str());
	}
}

fcyXmlNode fcyXml::parserNode(fcyLexicalReader& tReader)
{
	fcyXmlNode tNode;

	// 读取'<'
	tReader.Match(L'<', true);

	// 读取名称
	tNode.SetName(readName(tReader).c_str());

	tReader.IgnoreSpace();

	// 读取属性
	readAttribute(tReader, &tNode);

	fCharW tChar = tReader.ReadChar();
	if(tChar == L'/')
	{
		// 分支 <1> 遇到'/',完成节点解析

		// 读取'>'
		tReader.Match(L'>', false);
	}
	else if(tChar == L'>')
	{
		// 分支 <2> 解析子节点

		// 读取子节点或文本
		readNodes(tReader, &tNode);

		// 读取'</'
		tReader.Match(L"</", true);

		// 读取名称
		wstring tName = readName(tReader);

		if(tName != tNode.GetName())
		{
			fCharW tText[1024];
			swprintf_s(tText, L"Name <%s> not match <%s>.", tName.c_str(), tNode.GetName());
			throw fcyLexicalException("fcyXml::parserNode", 
				fcyStringHelper::WideCharToMultiByte(tText).c_str(),
				tReader.GetLine(), tReader.GetRow());
		}

		// 读取'>'
		tReader.Match(L'>', true);
	}
	else
	{
		fCharW tText[1024];
		swprintf_s(tText, L"Unexpected character '%c'.", tChar);
		throw fcyLexicalException("fcyXml::parserNode", 
			fcyStringHelper::WideCharToMultiByte(tText).c_str(),
			tReader.GetLine(), tReader.GetRow());
	}

	return tNode;
}

fcyXmlNode* fcyXml::GetRoot()
{
	return &m_pRoot;
}

void fcyXml::SetRoot(const fcyXmlNode& pNode)
{
	m_pRoot = pNode;
}

void fcyXml::WriteToStr(std::wstring& pOut)
{
	m_pRoot.writeToStr(pOut, 0);
}

void fcyXml::WriteToStream(fcyStream* pOut)
{
	wstring tOutStr;

	m_pRoot.writeToStr(tOutStr, 0);

	// UTF 16 BOM
	fByte tUTF16LE[2] = { 0xFF, 0xFE };
	pOut->WriteBytes(tUTF16LE, 2, NULL);
	pOut->WriteBytes((fData)&tOutStr[0], tOutStr.size() * 2, NULL);
}