#include "fuiContext.h"

#include "fcyMisc/fcyStringHelper.h"
#include "fcyOS/fcyDebug.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fcyStream* fuiContext::DefaultResLoader::openStream(fcStrW Path)
{
	try
	{
		fcyFileStream* pStream = new fcyFileStream(Path, false);
		return pStream;
	}
	catch(const fcyException& e)
	{
		FCYDEBUGEXCPT(e);
		return NULL;
	}
}

f2dTexture* fuiContext::DefaultResLoader::GetTexture(fcStrW Path)
{
	if(m_TexCache[Path]!=NULL)
		return m_TexCache[Path];

	fcyStream* pStream = openStream(Path);
	if(!pStream)
		return NULL;

	f2dTexture* pOut = NULL;
	if(FCYFAILED(m_pRenderer->GetDevice()->CreateTextureFromStream(pStream, 0, 0, false, true, &pOut)))
	{
		FCYSAFEKILL(pStream);
		return NULL;
	}
	FCYSAFEKILL(pStream);

	m_TexCache[Path] = pOut;
	return pOut;
}

f2dFontRenderer* fuiContext::DefaultResLoader::GetFont(fcStrW Path, fuInt Size)
{
	fcyStream* pStream = m_pFontFileCache[Path];
	if(!pStream)
	{
		m_pFontFileCache[Path] = openStream(Path);
		pStream = m_pFontFileCache[Path];
	}
	if(!pStream)
		return NULL;

	f2dFontRenderer* pFont = m_FontCache[Path][Size];
	if(!pFont)
	{
		m_pRenderer->CreateFontRenderer(pStream, Size, 0, 0, &pFont);
		m_FontCache[Path][Size] = pFont;
	}

	return pFont;
}

fuiContext::DefaultResLoader::DefaultResLoader(f2dRenderer* pRenderer)
	: m_pRenderer(pRenderer)
{}

fuiContext::DefaultResLoader::~DefaultResLoader()
{
	// 清除纹理缓存
	unordered_map<wstring, f2dTexture*>::iterator i =
		m_TexCache.begin();
	for(; i!=m_TexCache.end(); i++)
	{
		FCYSAFEKILL(i->second);
	}
	m_TexCache.clear();

	// 清除字体缓存
	unordered_map<wstring, unordered_map<fuInt, f2dFontRenderer*> >::iterator j = m_FontCache.begin();
	for(; j != m_FontCache.end(); j++)
	{
		unordered_map<fuInt, f2dFontRenderer*>::iterator i = (j->second).begin();
		for(; i != (j->second).end(); i++)
		{
			FCYSAFEKILL(i->second);
		}
	}
	m_FontCache.clear();

	// 清除文件
	unordered_map<wstring, fcyStream*>::iterator k = m_pFontFileCache.begin();
	for(; k != m_pFontFileCache.end(); k++)
	{
		FCYSAFEKILL(k->second);
	}
	m_pFontFileCache.clear();
}

////////////////////////////////////////////////////////////////////////////////

fuiContext::fuiContext(f2dRenderer* pRenderer)
	: m_DefaultLoader(pRenderer), m_ResLoader(&m_DefaultLoader), m_pRenderer(pRenderer)
{}

fuiContext::~fuiContext()
{
	ClearStyle();
	ClearSprite();
	ClearFontStyle();
}

fuiContextResLoader* fuiContext::GetResLoader()
{
	return m_ResLoader;
}

void fuiContext::SetResLoader(fuiContextResLoader* pLoader)
{
	m_ResLoader = pLoader;
}

void fuiContext::ClearSprite()
{
	unordered_map<wstring, f2dSprite*>::iterator i =
		m_SpriteCache.begin();
	for(; i!=m_SpriteCache.end(); i++)
	{
		FCYSAFEKILL(i->second);
	}
	m_SpriteCache.clear();
}

void fuiContext::ClearFontStyle()
{
	unordered_map<wstring, f2dFontRenderer*>::iterator i =
		m_FontCache.begin();
	for(; i!=m_FontCache.end(); i++)
	{
		FCYSAFEKILL(i->second);
	}
	m_FontCache.clear();
}

void fuiContext::ClearStyle()
{
	m_Style.clear();
}

void fuiContext::LoadSprite(fcyStream* pFile)
{
	if(!pFile)
		throw fcyException("fuiContext::LoadSprite", "Invalid stream pointer.");

	fcyXml tXml(pFile);

	LoadSprite(tXml.GetRoot());
}

void fuiContext::LoadSprite(fcyXmlNode* pNode)
{
	if(wcscmp(pNode->GetName(), L"fancyUISprite")!=0)
		throw fcyException("fuiContext::LoadSprite", "Not a proper file.");

	fcyXmlNode* pXmlNode = NULL;
	fuInt i = 0;
	while((pXmlNode = pNode->GetNodeByName(L"Sprite", i))!=NULL)
	{
		fcStrW tName = pXmlNode->GetAttribute(L"Name");
		fcStrW tSrc = pXmlNode->GetAttribute(L"Src");
		vector<wstring> tRectArr;
		fcyStringHelper::StringSplit(pXmlNode->GetAttribute(L"Rect"), L",", false, tRectArr);
		
		if(m_SpriteCache.find(tName) != m_SpriteCache.end())
			throw fcyException("fuiContext::LoadSprite", "Sprite has already defined.");

		if(tRectArr.size() != 4)
			throw fcyException("fuiContext::LoadSprite", "Rect is not correct.");

		f2dTexture* pTex = m_DefaultLoader.GetTexture(tSrc);
		if(!pTex)
			throw fcyException("fuiContext::LoadSprite", "Tex load failed.");

		fcyRect tRect(
			(float)_wtof(tRectArr[0].c_str()),
			(float)_wtof(tRectArr[1].c_str()),
			(float)_wtof(tRectArr[2].c_str()),
			(float)_wtof(tRectArr[3].c_str())
			);

		f2dSprite* tSprite = NULL;
		m_pRenderer->CreateSprite2D(pTex, tRect, &tSprite);
		m_SpriteCache[tName] = tSprite;

		i++;
	}
}

void fuiContext::LoadStyle(fcyStream* pFile)
{
	if(!pFile)
		throw fcyException("fuiContext::LoadStyle", "Invalid stream pointer.");

	fcyXml tXml(pFile);

	LoadStyle(tXml.GetRoot());
}

void fuiContext::LoadStyle(fcyXmlNode* pNode)
{
	if(wcscmp(pNode->GetName(), L"fancyUIStyle")!=0)
		throw fcyException("fuiContext::LoadStyle", "Not a proper file.");

	fcyXmlNode* pXmlNode = NULL;
	for(fuInt i = 0; i<pNode->GetNodeCount(); i++)
	{
		pXmlNode = pNode->GetNode(i);
		fcStrW tTypeName = pXmlNode->GetName();
		fcStrW tStyleName = pXmlNode->GetAttribute(L"StyleName");
		if(tTypeName == NULL || tStyleName == NULL ||
			wcslen(tTypeName)==0 || wcslen(tStyleName)==0)
		{
			throw fcyException("fuiContext::LoadStyle", "Invalid attribute.");
		}

		// 加入样式
		m_Style[tTypeName][tStyleName] = *pXmlNode;
	}
}

void fuiContext::LoadFontStyle(fcyStream* pFile)
{
	if(!pFile)
		throw fcyException("fuiContext::LoadFontStyle", "Invalid stream pointer.");

	fcyXml tXml(pFile);

	LoadFontStyle(tXml.GetRoot());
}

void fuiContext::LoadFontStyle(fcyXmlNode* pNode)
{
	if(wcscmp(pNode->GetName(), L"fancyUIFont")!=0)
		throw fcyException("fuiContext::LoadFontStyle", "Not a proper file.");

	fcyXmlNode* pXmlNode = NULL;
	fuInt i = 0;
	while((pXmlNode = pNode->GetNodeByName(L"Font", i))!=NULL)
	{
		fcStrW tName = pXmlNode->GetAttribute(L"Name");
		fcStrW tSrc = pXmlNode->GetAttribute(L"Src");
		fuInt tSize = _wtoi(pXmlNode->GetAttribute(L"Size"));
		
		if(tSize == 0)
			throw fcyException("fuiContext::LoadFontStyle", "Font size invalid.");

		if(m_FontCache.find(tName) != m_FontCache.end())
			throw fcyException("fuiContext::LoadFontStyle", "Font style has already defined.");

		f2dFontRenderer* pFont = m_DefaultLoader.GetFont(tSrc, tSize);
		if(!pFont)
			throw fcyException("fuiContext::LoadFontStyle", "Font load failed.");

		m_FontCache[tName] = pFont;
		pFont->AddRef();

		i++;
	}
}

f2dSprite* fuiContext::GetSprite(fcStrW SpriteName)
{
	return m_SpriteCache[SpriteName];
}

fcyXmlNode* fuiContext::GetStyle(fcStrW ControlType, fcStrW StyleName)
{
	return &m_Style[ControlType][StyleName];
}

f2dFontRenderer* fuiContext::GetFont(fcStrW FontName)
{
	return m_FontCache[FontName];
}
