#include "f2dFontTexProvider.h"

#include <fcyMisc/fcyStringHelper.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

f2dFontTexProvider::f2dFontTexProvider(f2dStream* pDefineFile, f2dTexture2D* pTex)
	: m_OrgTex(pTex)
{
	if(!m_OrgTex)
		throw fcyException("f2dFontTexProvider::f2dFontTexProvider", "Param 'pTex' is null.");
	fcyXml tXml(pDefineFile);

	loadDefine(tXml);

	m_OrgTex->AddRef();
}

f2dFontTexProvider::f2dFontTexProvider(fcStrW pDefineFile, f2dTexture2D* pTex)
	: m_OrgTex(pTex)
{
	if(!m_OrgTex)
		throw fcyException("f2dFontTexProvider::f2dFontTexProvider", "Param 'pTex' is null.");
	fcyXml tXml(pDefineFile);

	loadDefine(tXml);

	m_OrgTex->AddRef();
}

f2dFontTexProvider::~f2dFontTexProvider()
{
	FCYSAFEKILL(m_OrgTex);
}

fcyVec2 f2dFontTexProvider::readVec2Str(const std::wstring& Str)
{
	vector<wstring> tOut;
	if(2 != fcyStringHelper::StringSplit(Str, L",", false, tOut))
		throw fcyException("f2dFontTexProvider::readVec2Str", "String format error.");

	return fcyVec2((float)_wtof(tOut[0].c_str()), (float)_wtof(tOut[1].c_str()));
}

void f2dFontTexProvider::loadDefine(fcyXml& Xml)
{
	float tXScale = 1.f / m_OrgTex->GetWidth();
	float tYScale = 1.f / m_OrgTex->GetHeight();

	fcyXmlNode* pRoot = Xml.GetRoot();

	if(wcscmp(pRoot->GetName(), L"f2dTexturedFont") != 0)
		throw fcyException("f2dFontTexProvider::loadDefine", "Invalid file, root node name not match.");
	
	fcyXmlNode* pMeasureNode = pRoot->GetNodeByName(L"Measure", 0);
	if(!pMeasureNode)
		throw fcyException("f2dFontTexProvider::loadDefine", "Invalid file, node 'Measure' not found.");

	fcyXmlNode* pCharList = pRoot->GetNodeByName(L"CharList", 0);

	// 读取度量值
	m_LineHeight = (float)_wtof(pMeasureNode->GetAttribute(L"LineHeight"));
	m_Ascender = (float)_wtof(pMeasureNode->GetAttribute(L"Ascender"));
	m_Descender = (float)_wtof(pMeasureNode->GetAttribute(L"Descender"));

	// 读取字符表
	fuInt tSubNodeCount = pCharList->GetNodeCount(L"Item");
	for(fuInt i = 0; i<tSubNodeCount; ++i)
	{
		fcyXmlNode* pSub = pCharList->GetNodeByName(L"Item", i);

		fcStrW tChar = pSub->GetAttribute(L"Char");
		if(wcslen(tChar) != 1)
			throw fcyException("f2dFontTexProvider::loadDefine", "Invalid file, invalid character in CharList.");
		
		f2dGlyphInfo tInfo;
		tInfo.Advance = readVec2Str(pSub->GetAttribute(L"Advance"));
		tInfo.BrushPos = readVec2Str(pSub->GetAttribute(L"BrushPos"));
		tInfo.GlyphSize = readVec2Str(pSub->GetAttribute(L"Size"));
		tInfo.GlyphPos.a = readVec2Str(pSub->GetAttribute(L"Pos"));
		tInfo.GlyphPos.b = tInfo.GlyphPos.a + readVec2Str(pSub->GetAttribute(L"Size"));
		
		tInfo.GlyphPos.a.x *= tXScale;
		tInfo.GlyphPos.a.y *= tYScale;
		tInfo.GlyphPos.b.x *= tXScale;
		tInfo.GlyphPos.b.y *= tYScale;

		m_Cache[tChar[0]] = tInfo;
	}
}

fResult f2dFontTexProvider::QueryGlyph(f2dGraphics* pGraph, fCharW Character, f2dGlyphInfo* InfoOut)
{
	unordered_map<fCharW, f2dGlyphInfo>::iterator i = m_Cache.find(Character);

	if(i == m_Cache.end())
		return FCYERR_OBJNOTEXSIT;

	*InfoOut = i->second;

	return FCYERR_OK;
}
