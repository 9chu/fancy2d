#include "fuiRes.h"

#include "fuiGraphics.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiResProviderImpl::fuiResProviderImpl()
	: m_pFileSys(NULL), m_pRenderer(NULL)
{}

fuiResProviderImpl::fuiResProviderImpl(f2dFileSys* pFileSys, f2dRenderer* pRenderer)
	: m_pFileSys(pFileSys), m_pRenderer(pRenderer)
{}

fuiResProviderImpl::fuiResProviderImpl(const fuiResProviderImpl& Org)
{
	m_pFileSys = Org.m_pFileSys;
	m_pRenderer = Org.m_pRenderer;
	m_TexCache = Org.m_TexCache;
	m_FontCache = Org.m_FontCache;
	m_SpriteFontCache = Org.m_SpriteFontCache;
}

f2dTexture2D* fuiResProviderImpl::QueryTexture(const std::wstring& ResSrc)
{
	if(!m_pFileSys || !m_pRenderer)
		throw fcyException("fuiResProviderImpl::QueryTexture", "Null pointer.");

	unordered_map<std::wstring, fcyRefPointer<f2dTexture2D>>::iterator i = m_TexCache.find(ResSrc);

	if(i == m_TexCache.end())
	{
		// 获取流
		f2dStream* pStream = m_pFileSys->GetStream(ResSrc.c_str());
		if(!pStream)
			throw fcyException("fuiResProviderImpl::QueryTexture", "Stream not found.");
		pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

		// 创建纹理
		f2dTexture2D* pTex = NULL;
		if(FCYFAILED(m_pRenderer->GetDevice()->CreateTextureFromStream(pStream, 0, 0, false, true, &pTex)))
		{
			throw fcyException("fuiResProviderImpl::QueryTexture", "f2dRenderer::CreateTextureFromStream failed.");
		}

		m_TexCache[ResSrc].DirectSet(pTex);

		return pTex;
	}
	else
		return i->second;
}

f2dFontProvider* fuiResProviderImpl::QueryFont(const std::wstring& ResSrc, const fcyVec2& Size)
{
	if(!m_pFileSys || !m_pRenderer)
		throw fcyException("fuiResProviderImpl::QueryFont", "Null pointer.");

	unordered_map<std::pair<std::wstring, fcyVec2>, fcyRefPointer<f2dFontProvider>>::iterator i = m_FontCache.find(pair<wstring, fcyVec2>(ResSrc, Size));

	if(i == m_FontCache.end())
	{
		// 获取流
		f2dStream* pStream = m_pFileSys->GetStream(ResSrc.c_str());
		if(!pStream)
			throw fcyException("fuiResProviderImpl::QueryFont", "Stream not found.");
		pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

		// 创建字体
		f2dFontProvider* pFont = NULL;
		if(FCYFAILED(m_pRenderer->CreateFontFromFile(pStream, 0, Size, F2DFONTFLAG_NONE, &pFont)))
		{
			throw fcyException("fuiResProviderImpl::QueryFont", "f2dRenderer::CreateFontFromFile failed.");
		}

		m_FontCache[pair<std::wstring, fcyVec2>(ResSrc, Size)].DirectSet(pFont);
		
		return pFont;
	}
	else
		return i->second;
}

f2dFontProvider* fuiResProviderImpl::QuerySpriteFont(const std::wstring& TexSrc, const std::wstring& DefineSrc)
{
	if(!m_pFileSys || !m_pRenderer)
		throw fcyException("fuiResProviderImpl::QuerySpriteFont", "Null pointer.");

	unordered_map<std::pair<std::wstring, std::wstring>, fcyRefPointer<f2dFontProvider>>::iterator i = m_SpriteFontCache.find(pair<wstring, wstring>(TexSrc, DefineSrc));

	if(i == m_SpriteFontCache.end())
	{
		// 查询纹理
		f2dTexture2D* pTex = QueryTexture(TexSrc);

		// 获取流
		f2dStream* pStream = m_pFileSys->GetStream(DefineSrc.c_str());
		if(!pStream)
			throw fcyException("fuiResProviderImpl::QuerySpriteFont", "Stream not found.");
		pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

		// 创建字体
		f2dFontProvider* pFont = NULL;
		if(FCYFAILED(m_pRenderer->CreateFontFromTex(pStream, pTex, &pFont)))
		{
			throw fcyException("fuiResProviderImpl::QuerySpriteFont", "f2dRenderer::CreateFontFromTex failed.");
		}

		m_SpriteFontCache[pair<std::wstring, std::wstring>(TexSrc, DefineSrc)].DirectSet(pFont);
		
		return pFont;
	}
	else
		return i->second;
}

////////////////////////////////////////////////////////////////////////////////

fuiRes::fuiRes(RESTYPE Type, const std::wstring& Name)
	: m_Type(Type), m_ResName(Name)
{
	m_Type_Accessor = fuiPropertyAccessor<RESTYPE>(
		&m_Type,
		[&](std::wstring& Prop, const RESTYPE* Value)
		{
			fuiPropertyAccessorHelper<int>::DefaultGetter(Prop, (int*)Value);
		},
		fuiPropertyAccessor<RESTYPE>::PropSetter()
	);
	m_ResName_Accessor = fuiPropertyAccessor<std::wstring>(
		&m_ResName,
		fuiPropertyAccessorHelper<std::wstring>::DefaultGetter,
		fuiPropertyAccessor<std::wstring>::PropSetter()
	);

	RegisterProperty(L"Type", &m_Type_Accessor);
	RegisterProperty(L"Name", &m_ResName_Accessor);
}

////////////////////////////////////////////////////////////////////////////////

fuiSprite::fuiSprite(const std::wstring& Name)
	: fuiRes(RESTYPE_SPRITE, Name), m_bSetHotPos(false), m_bSetRect(false)
{
	m_TexPath_Accessor = fuiPropertyAccessor<std::wstring>(&m_TexPath);
	m_Rect_Accessor = fuiPropertyAccessor<fcyRect>(
		&m_Rect,
		fuiPropertyAccessorHelper<fcyRect>::DefaultGetter,
		[&](const std::wstring& Prop, fcyRect* Value)
		{
			fuiPropertyAccessorHelper<fcyRect>::DefaultSetter(Prop, Value);
			m_bSetRect = true;
		}
	);
	m_HotPos_Accessor = fuiPropertyAccessor<fcyVec2>(
		&m_HotPos,
		fuiPropertyAccessorHelper<fcyVec2>::DefaultGetter,
		[&](const std::wstring& Prop, fcyVec2* Value)
		{
			fuiPropertyAccessorHelper<fcyVec2>::DefaultSetter(Prop, Value);
			m_bSetHotPos = true;
		}
	);

	RegisterProperty(L"Src", &m_TexPath_Accessor);
	RegisterProperty(L"Rect", &m_Rect_Accessor);
	RegisterProperty(L"HotPos", &m_HotPos_Accessor);
}

void fuiSprite::ConstructRes(fuiResProvider* pProvider)
{
	if(!pProvider)
		throw fcyException("fuiSprite::ConstructRes", "Param 'pProvider' is null.");
	f2dRenderer* pRenderer = pProvider->GetRenderer();
	if(!pRenderer)
		throw fcyException("fuiSprite::ConstructRes", "fuiResProvider::GetRenderer return null pointer.");

	// 构造资源
	f2dTexture2D* pTex = pProvider->QueryTexture(m_TexPath);
	if(!m_bSetRect)
		m_Rect = fcyRect(0.f, 0.f, (float)pTex->GetWidth(), (float)pTex->GetHeight());
	if(!m_bSetHotPos)
		m_HotPos = m_Rect.GetCenter();

	if(FCYFAILED(pRenderer->CreateSprite2D(pTex, m_Rect, m_HotPos, &m_pSprite)))
		throw fcyException("fuiSprite::ConstructRes", "f2dRenderer::CreateSprite2D failed.");
}

////////////////////////////////////////////////////////////////////////////////

fuiBorderSprite::fuiBorderSprite(const std::wstring& Name)
	: fuiRes(RESTYPE_BORDERSPRITE, Name)
{
	m_TexPath_Accessor = fuiPropertyAccessor<std::wstring>(&m_TexPath);
	m_Rect_Accessor = fuiPropertyAccessor<fcyRect>(&m_Rect);
	m_Margin_Accessor = fuiPropertyAccessor<fcyRect>(&m_Margin);

	RegisterProperty(L"Src", &m_TexPath_Accessor);
	RegisterProperty(L"Rect", &m_Rect_Accessor);
	RegisterProperty(L"Margin", &m_Margin_Accessor);
}

void fuiBorderSprite::Draw(fuiGraphics* pGraph, const fcyRect& Pos, const fcyColor& Color)
{
	if(!m_pBorderSprite)
		throw fcyException("fuiBorderSprite::Draw", "Uninit object.");
	if(!pGraph)
		throw fcyException("fuiBorderSprite::Draw", "Param 'pGraph' is null.");

	f2dGraphics2D* pRealGraph = pGraph->GetGraphics();

	m_pBorderSprite->SetColor(Color);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x,                Pos.a.y, Pos.a.x + m_Margin.a.x, Pos.a.y + m_Margin.a.y), m_Sprites[0][0]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x + m_Margin.a.x, Pos.a.y, Pos.b.x - m_Margin.b.x, Pos.a.y + m_Margin.a.y), m_Sprites[0][1]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.b.x - m_Margin.b.x, Pos.a.y, Pos.b.x,                Pos.a.y + m_Margin.a.y), m_Sprites[0][2]);

	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x,                Pos.a.y + m_Margin.a.y, Pos.a.x + m_Margin.a.x, Pos.b.y - m_Margin.b.y), m_Sprites[1][0]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x + m_Margin.a.x, Pos.a.y + m_Margin.a.y, Pos.b.x - m_Margin.b.y, Pos.b.y - m_Margin.b.y), m_Sprites[1][1]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.b.x - m_Margin.b.x, Pos.a.y + m_Margin.a.y, Pos.b.x,                Pos.b.y - m_Margin.b.y), m_Sprites[1][2]);
	
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x,                Pos.b.y - m_Margin.b.y, Pos.a.x + m_Margin.a.x, Pos.b.y), m_Sprites[2][0]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.a.x + m_Margin.a.x, Pos.b.y - m_Margin.b.y, Pos.b.x - m_Margin.b.y, Pos.b.y), m_Sprites[2][1]);
	m_pBorderSprite->Draw(pRealGraph, fcyRect(Pos.b.x - m_Margin.b.x, Pos.b.y - m_Margin.b.y, Pos.b.x,                Pos.b.y), m_Sprites[2][2]);
}

void fuiBorderSprite::ConstructRes(fuiResProvider* pProvider)
{
	if(!pProvider)
		throw fcyException("fuiBorderSprite::ConstructRes", "Param 'pProvider' is null.");
	f2dRenderer* pRenderer = pProvider->GetRenderer();
	if(!pRenderer)
		throw fcyException("fuiBorderSprite::ConstructRes", "fuiResProvider::GetRenderer return null pointer.");

	// 构造原始精灵
	f2dTexture2D* pTex = pProvider->QueryTexture(m_TexPath);
	if(FCYFAILED(pRenderer->CreateSprite2D(pTex, m_Rect, &m_pBorderSprite)))
		throw fcyException("fuiBorderSprite::ConstructRes", "f2dRenderer::CreateSprite2D failed.");

	fcyRect tUVCoord = m_Margin;
	tUVCoord.b = m_Rect.b - m_Rect.a - tUVCoord.b;
	tUVCoord.a.x /= m_Rect.GetWidth();
	tUVCoord.b.x /= m_Rect.GetWidth();
	tUVCoord.a.y /= m_Rect.GetHeight();
	tUVCoord.b.y /= m_Rect.GetHeight();

	m_Sprites[0][0] = fcyRect(0.f,          0.f, tUVCoord.a.x, tUVCoord.a.y); // 左上
	m_Sprites[0][1] = fcyRect(tUVCoord.a.x, 0.f, tUVCoord.b.x, tUVCoord.a.y); // 上
	m_Sprites[0][2] = fcyRect(tUVCoord.b.x, 0.f, 1.f         , tUVCoord.a.y); // 右上

	m_Sprites[1][0] = fcyRect(0.f,          tUVCoord.a.y, tUVCoord.a.x, tUVCoord.b.y); // 左中
	m_Sprites[1][1] = fcyRect(tUVCoord.a.x, tUVCoord.a.y, tUVCoord.b.x, tUVCoord.b.y); // 中
	m_Sprites[1][2] = fcyRect(tUVCoord.b.x, tUVCoord.a.y, 1.f         , tUVCoord.b.y); // 右中

	m_Sprites[2][0] = fcyRect(0.f,          tUVCoord.b.y, tUVCoord.a.x, 1.f); // 左下
	m_Sprites[2][1] = fcyRect(tUVCoord.a.x, tUVCoord.b.y, tUVCoord.b.x, 1.f); // 下
	m_Sprites[2][2] = fcyRect(tUVCoord.b.x, tUVCoord.b.y, 1.f         , 1.f); // 右下
}

////////////////////////////////////////////////////////////////////////////////

fuiFont::fuiFont(const std::wstring& Name)
	: fuiRes(RESTYPE_FONT, Name)
{
	m_FontSrc_Accessor = fuiPropertyAccessor<wstring>(&m_FontSrc);
	m_TexSrc_Accessor = fuiPropertyAccessor<wstring>(&m_TexSrc);
	m_Define_Accessor = fuiPropertyAccessor<wstring>(&m_Define);
	m_FontSize_Accessor = fuiPropertyAccessor<fcyVec2>(&m_FontSize);

	RegisterProperty(L"FontSrc", &m_FontSrc_Accessor);
	RegisterProperty(L"TexSrc", &m_TexSrc_Accessor);
	RegisterProperty(L"Define", &m_Define_Accessor);
	RegisterProperty(L"FontSize", &m_FontSize_Accessor);
}

void fuiFont::ConstructRes(fuiResProvider* pProvider)
{
	if(!pProvider)
		throw fcyException("fuiFont::ConstructRes", "Param 'pProvider' is null.");
	f2dRenderer* pRenderer = pProvider->GetRenderer();
	if(!pRenderer)
		throw fcyException("fuiFont::ConstructRes", "fuiResProvider::GetRenderer return null pointer.");

	if(!m_FontSrc.empty() && (!m_Define.empty() || !m_TexSrc.empty()))
		throw fcyException("fuiFont::ConstructRes", "Font or SpriteFont?");

	if(!m_FontSrc.empty())
		m_pProvider = pProvider->QueryFont(m_FontSrc, m_FontSize);
	else
		m_pProvider = pProvider->QuerySpriteFont(m_TexSrc, m_Define);

	if(FCYFAILED(pRenderer->CreateFontRenderer(m_pProvider, &m_pRenderer)))
		throw fcyException("fuiFont::ConstructRes", "f2dRenderer::CreateFontRenderer failed.");
}
