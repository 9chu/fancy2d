#include "f2dFontFileProvider.h"

#include <fcyException.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const int f2dFontFileProvider::s_Magin = 1;

f2dFontFileProvider::f2dFontFileProvider(f2dRenderDevice* pParent, f2dStream* pStream, const fcyVec2& FontSize, fuInt FaceIndex, F2DFONTFLAG Flag)
	: m_pParent(pParent), m_pStream(pStream),
	m_CacheXCount(0), m_CacheYCount(0), m_Cache(NULL), m_CacheTex(NULL),
	m_UsedNodeList(NULL), m_FreeNodeList(NULL)
{
	// --- ��ʼ��freetype ---
	FT_Error tErr = FT_Init_FreeType( &m_FontLib );
	if(tErr)
		throw fcyException("f2dFontFileProvider::f2dFontFileProvider", "FT_Init_FreeType failed.");

	// --- ׼���� ---
	pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);
	memset(&m_Args, 0, sizeof(m_Args));
	memset(&m_Stream, 0, sizeof(m_Stream));
	m_Args.flags = FT_OPEN_STREAM;
	m_Args.stream = &m_Stream;
	m_Stream.size = (unsigned long)m_pStream->GetLength();
	m_Stream.descriptor.pointer = m_pStream;
	m_Stream.read = streamRead;
	m_Stream.close = streamClose;
	m_pStream->AddRef();

	// --- �������� ---
	tErr = FT_Open_Face(m_FontLib, &m_Args, FaceIndex, &m_Face);
	if(tErr)
	{
		FT_Done_FreeType(m_FontLib);

		FCYSAFEKILL(m_pStream);

		throw fcyException("f2dFontFileProvider::f2dFontFileProvider", "FT_New_Memory_Face failed.");
	}

	// --- ���������С ---
	FT_Set_Pixel_Sizes(m_Face, (FT_UInt)FontSize.x, (FT_UInt)FontSize.y);

	// --- ���������� ---
	// ��������С
	m_PerGlyphSize.x = ceil(widthSizeToPixel(abs(m_Face->bbox.xMax - m_Face->bbox.xMin)) + s_Magin * 2.f);
	m_PerGlyphSize.y = ceil(heightSizeToPixel(abs(m_Face->bbox.yMax - m_Face->bbox.yMin)) + s_Magin * 2.f);

	// �������е�������
	if(1024.f / m_PerGlyphSize.x < 10.f || 1024.f / m_PerGlyphSize.y < 10.f)
	{
		// ����10x10������
		if(2048.f / m_PerGlyphSize.x < 1.f || 2048.f / m_PerGlyphSize.y < 1.f)
		{
			// ����һ������
			FT_Done_Face(m_Face);
			FT_Done_FreeType(m_FontLib);

			FCYSAFEKILL(m_pStream);

			throw fcyException("f2dFontFileProvider::f2dFontFileProvider", "Font size is too big.");
		}
		else
		{
			m_TexSize = 2048;
			m_CacheXCount = (int)(2048.f / m_PerGlyphSize.x);
			m_CacheYCount = (int)(2048.f / m_PerGlyphSize.y);
		}
	}
	else
	{
		m_TexSize = 1024;
		m_CacheXCount = (int)(1024.f / m_PerGlyphSize.x);
		m_CacheYCount = (int)(1024.f / m_PerGlyphSize.y);
	}

	// �������建������
	if(!makeCache(m_TexSize))
	{
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_FontLib);

		FCYSAFEKILL(m_pStream);

		throw fcyException("f2dFontFileProvider::f2dFontFileProvider", "makeCache failed.");
	}

	// --- �󶨼����� ---
	m_pParent->AttachListener(this);
}

f2dFontFileProvider::~f2dFontFileProvider()
{
	FT_Done_Face(m_Face);
	FT_Done_FreeType(m_FontLib);

	// �ͷ����建��
	FCYSAFEDELARR(m_Cache);
	FCYSAFEKILL(m_CacheTex);

	// �ͷ���
	FCYSAFEKILL(m_pStream);

	// ����������
	m_pParent->RemoveListener(this);
}

void f2dFontFileProvider::addUsedNode(FontCacheInfo* p)
{
	// ���ӱ��ڵ�
	if(m_UsedNodeList)
	{
		p->pPrev = m_UsedNodeList->pPrev;
		p->pNext = m_UsedNodeList;
	}
	else
	{
		p->pPrev = p->pNext = p;
	}

	// ���뱾�ڵ�
	if(m_UsedNodeList)
	{
		m_UsedNodeList->pPrev->pNext = p;
		m_UsedNodeList->pPrev = p;
	}
	m_UsedNodeList = p;
}

void f2dFontFileProvider::removeFreeNode(FontCacheInfo* p)
{
	// ����Ƿ�Ϊ��ͷ
	if(p == m_FreeNodeList)
		m_FreeNodeList = p->pNext;

	// �Ƴ����������
	if(p->pPrev)
		p->pPrev->pNext = p->pNext;
	if(p->pNext)
		p->pNext->pPrev = p->pPrev;

	// ���ֵ
	p->pNext = p->pPrev = NULL;
}

void f2dFontFileProvider::removeUsedNode(FontCacheInfo* p)
{
	// �Ƿ����׽ڵ�
	if(p == m_UsedNodeList)
		if(p->pNext == p)
			m_UsedNodeList = NULL;
		else
			m_UsedNodeList = p->pNext;

	// �Ƴ����������
	p->pNext->pPrev = p->pPrev;
	p->pPrev->pNext = p->pNext;

	p->pPrev = p->pNext = NULL;
}

f2dFontFileProvider::FontCacheInfo* f2dFontFileProvider::getChar(fCharW Char)
{
	// �����ֵ�����Ѱ��
	FontCacheInfo* pCache = m_Dict[Char];

	// �ҵ��Ļ�ֱ�ӷ���
	if(pCache)
	{
		// �ҵ�UsedList�ж�Ӧ��λ�ã���ɾ���ڵ㣬�����Ƶ���λ
		removeUsedNode(pCache);
		addUsedNode(pCache);

		return pCache;
	}

	// �������Ƿ��п�λ��������
	if(m_FreeNodeList)
	{
		// �п�λ
		pCache = m_FreeNodeList;

		// ���б����Ƴ�
		removeFreeNode(pCache);

		// ��������
		renderCache(pCache, Char);

		// ��¼
		m_Dict[Char] = pCache;

		// ����MRU�б�
		addUsedNode(pCache);

		// ����
		return pCache;
	}

	// û�п�λ��
	{
		// ��UsedList�����ȡ��
		pCache = m_UsedNodeList->pPrev;

		// ��ն�Ӧ�Ļ�������
		m_Dict[pCache->Character] = NULL;

		// �Ƴ�
		removeUsedNode(pCache);

		// ����
		renderCache(pCache, Char);

		// ��¼
		m_Dict[Char] = pCache;

		// ����MRU�б�
		addUsedNode(pCache);

		// ����
		return pCache;
	}
}

bool f2dFontFileProvider::makeCache(fuInt Size)
{
	// �����������
	m_Dict.clear();
	FCYSAFEDELARR(m_Cache);
	FCYSAFEKILL(m_CacheTex);
	m_UsedNodeList = NULL;

	// �������建����
	if(FCYFAILED(m_pParent->CreateDynamicTexture(Size, Size, &m_CacheTex)))
		return false;
	// ��ջ�����
	{
		fuInt tPitch = 0;
		fData tData = NULL;
		if(FCYFAILED(m_CacheTex->Lock(NULL, true, &tPitch, &tData)))
			return false;

		for(fuInt i = 0;i<m_CacheTex->GetHeight(); ++i)
		{
			fcyColor* pColor = (fcyColor*)tData;
			for(fuInt j = 0; j<m_CacheTex->GetWidth(); j++)
			{
				pColor[j].argb = 0x00000000;
			}
			tData += tPitch;
		}

		m_CacheTex->Unlock();
	}
	
	fuInt tCount = m_CacheXCount * m_CacheYCount;
	m_Cache = new FontCacheInfo[ tCount ];

	m_FreeNodeList = m_Cache;

	// ��ʼ����Ϣ
	int tCurIndex = 0;
	for(fuInt j = 0; j<m_CacheYCount; j++)
		for(fuInt i = 0; i<m_CacheXCount; ++i)
		{
			m_Cache[ tCurIndex ].CacheSize = fcyRect(
					i * m_PerGlyphSize.x, j * m_PerGlyphSize.y, 
					(i+1) * m_PerGlyphSize.x, (j+1) * m_PerGlyphSize.y
					);
			
			m_Cache[ tCurIndex ].Character = L'\0';
			
			tCurIndex++;
		}

	// ���ӻ�����
	m_Cache[0].pPrev = NULL;
	m_Cache[0].pNext = &m_Cache[1];
	m_Cache[tCount - 1].pNext = NULL;
	m_Cache[tCount - 1].pPrev = &m_Cache[ tCount - 2 ];
	for(fuInt i = 1; i<tCount - 1; ++i)
	{
		m_Cache[i].pPrev = &m_Cache[i - 1];
		m_Cache[i].pNext = &m_Cache[i + 1];
	}

	return true;
}

bool f2dFontFileProvider::renderCache(FontCacheInfo* pCache, fCharW Char)
{
	// �������ֵ����β۲���Ⱦ
	FT_Load_Char( m_Face, Char, FT_LOAD_RENDER );

	// ���ͼ��
	FT_Bitmap* tBitmap = &m_Face->glyph->bitmap;

	// д���Ӧ����
	pCache->Character = Char;
	pCache->Advance = fcyVec2(m_Face->glyph->advance.x / 64.f, m_Face->glyph->advance.y / 64.f);
	pCache->BrushPos = fcyVec2(-(float)m_Face->glyph->bitmap_left, (float)m_Face->glyph->bitmap_top);
	pCache->UV = fcyRect(
		pCache->CacheSize.a + fcyVec2((float)s_Magin, (float)s_Magin),
		pCache->CacheSize.a + fcyVec2((float)tBitmap->width, (float)tBitmap->rows) + fcyVec2((float)s_Magin, (float)s_Magin)
		);
	pCache->GlyphSize = fcyVec2((float)m_Face->glyph->bitmap.width, (float)m_Face->glyph->bitmap.rows); 
	
	// ת��UV����
	fcyVec2 tUVScale(1.f / (float)m_CacheTex->GetWidth(), 1.f / (float)m_CacheTex->GetHeight());
	pCache->UV.a.x *= tUVScale.x;
	pCache->UV.a.y *= tUVScale.y;
	pCache->UV.b.x *= tUVScale.x;
	pCache->UV.b.y *= tUVScale.y;

	// ��������Ӧλ��
	{
		fuInt tPitch = 0;
		fData tData = NULL;
		fData tSrcData = tBitmap->buffer;
		if(FCYFAILED(m_CacheTex->Lock(&pCache->CacheSize, false, &tPitch, &tData)))
			return false;
		
		int tHeight = (int)pCache->CacheSize.GetHeight();
		int tWidth = (int)pCache->CacheSize.GetWidth();

		for(int y = 0; y<tHeight; y++)
		{
			for(int x = 0; x<tWidth; x++)
			{
				fcyColor* pColor = (fcyColor*)tData;
				
				pColor[x].argb = 0x00000000;

				// ������������
				if (y >= s_Magin && y < s_Magin + tBitmap->rows)
				{
					if (x >= s_Magin && x < s_Magin + tBitmap->width)
						pColor[x].a = tSrcData[x - s_Magin];
				}	
			}

			if(y >= s_Magin && y < s_Magin + tBitmap->rows)
				tSrcData += tBitmap->pitch;
			tData += tPitch;
		}

		m_CacheTex->Unlock();
	}

	return true;
}

void f2dFontFileProvider::OnRenderDeviceLost()
{
	m_Dict.clear();
	
	// ���������ѻ��������
	m_CharInUsing.clear();
	m_CharInUsing.reserve(m_CacheXCount * m_CacheYCount + 1);

	FontCacheInfo* pCache = m_UsedNodeList;
	do
	{
		if(pCache && pCache->Character != L'\0')
		{
			m_CharInUsing += pCache->Character;
			pCache = pCache->pNext;
		}
		else
			break;
	}
	while(pCache != m_UsedNodeList);
}

void f2dFontFileProvider::OnRenderDeviceReset()
{
	makeCache(m_TexSize);

	// ���»���
	CacheString(m_CharInUsing.c_str());
	m_CharInUsing.clear();
	m_CharInUsing.reserve(0);
}

fResult f2dFontFileProvider::CacheString(fcStrW String)
{
	fuInt tLen = wcslen(String);

	for(fuInt i = 0; i<tLen; ++i)
	{
		if(iswprint(String[i]))
			getChar(String[i]);
	}

	return FCYERR_OK;
}

fResult f2dFontFileProvider::QueryGlyph(f2dGraphics* pGraph, fCharW Character, f2dGlyphInfo* InfoOut)
{
	if(Character != L'\t' && iswcntrl(Character))
		return FCYERR_INVAILDPARAM;
	if(!InfoOut)
		return FCYERR_INVAILDPARAM;

	if(Character == L'\t')
	{
		fResult tRet = QueryGlyph(pGraph, L' ', InfoOut);
		if(FCYOK(tRet))
			InfoOut->Advance.x *= 2.f;
		return tRet;
	}

	if(pGraph)  // ���ƹ���
	{
		// ����Ƿ���ҪFlush
		if(m_FreeNodeList == NULL && !m_Dict[Character])
			pGraph->Flush();
	}

	// ȡ������
	FontCacheInfo* pCache = getChar(Character);

	InfoOut->Advance = pCache->Advance;
	InfoOut->BrushPos = pCache->BrushPos;
	InfoOut->GlyphPos = pCache->UV;
	InfoOut->GlyphSize = pCache->GlyphSize;

	return FCYERR_OK;
}
