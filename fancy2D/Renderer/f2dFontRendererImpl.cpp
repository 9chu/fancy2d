#include "f2dFontRendererImpl.h"

#include <fcyException.h>

////////////////////////////////////////////////////////////////////////////////

f2dFontRendererImpl::f2dFontRendererImpl(f2dFontProvider* pProvider)
	: m_pListener(NULL), m_pProvider(pProvider), m_FlipType(F2DSPRITEFLIP_NONE), m_ZValue(1.f), m_Scale(1.f, 1.f)
{
	if (m_pProvider)
		m_pProvider->AddRef();
}

f2dFontRendererImpl::~f2dFontRendererImpl()
{
	FCYSAFEKILL(m_pProvider);
}

void f2dFontRendererImpl::copyAndFlipUV(const f2dGlyphInfo& Info, f2dGraphics2DVertex DestArr[])
{
	switch (m_FlipType)
	{
	case F2DSPRITEFLIP_H:
		DestArr[1].u = Info.GlyphPos.a.x;
		DestArr[0].v = Info.GlyphPos.a.y;
		DestArr[0].u = Info.GlyphPos.b.x;
		DestArr[1].v = Info.GlyphPos.a.y;
		DestArr[3].u = Info.GlyphPos.b.x;
		DestArr[2].v = Info.GlyphPos.b.y;
		DestArr[2].u = Info.GlyphPos.a.x;
		DestArr[3].v = Info.GlyphPos.b.y;
		break;
	case F2DSPRITEFLIP_V:
		DestArr[0].u = Info.GlyphPos.a.x;
		DestArr[3].v = Info.GlyphPos.a.y;
		DestArr[1].u = Info.GlyphPos.b.x;
		DestArr[2].v = Info.GlyphPos.a.y;
		DestArr[2].u = Info.GlyphPos.b.x;
		DestArr[1].v = Info.GlyphPos.b.y;
		DestArr[3].u = Info.GlyphPos.a.x;
		DestArr[0].v = Info.GlyphPos.b.y;
		break;
	case F2DSPRITEFLIP_HV:
		DestArr[2].u = Info.GlyphPos.a.x;
		DestArr[2].v = Info.GlyphPos.a.y;
		DestArr[3].u = Info.GlyphPos.b.x;
		DestArr[3].v = Info.GlyphPos.a.y;
		DestArr[0].u = Info.GlyphPos.b.x;
		DestArr[0].v = Info.GlyphPos.b.y;
		DestArr[1].u = Info.GlyphPos.a.x;
		DestArr[1].v = Info.GlyphPos.b.y;
		break;
	default:
		DestArr[0].u = Info.GlyphPos.a.x;
		DestArr[0].v = Info.GlyphPos.a.y;
		DestArr[1].u = Info.GlyphPos.b.x;
		DestArr[1].v = Info.GlyphPos.a.y;
		DestArr[2].u = Info.GlyphPos.b.x;
		DestArr[2].v = Info.GlyphPos.b.y;
		DestArr[3].u = Info.GlyphPos.a.x;
		DestArr[3].v = Info.GlyphPos.b.y;
		break;
	}
}

f2dFontRendererListener* f2dFontRendererImpl::GetListener()
{
	return m_pListener;
}

void f2dFontRendererImpl::SetListener(f2dFontRendererListener* pListener)
{
	m_pListener = pListener;
}

f2dFontProvider* f2dFontRendererImpl::GetFontProvider()
{
	return m_pProvider;
}

fResult f2dFontRendererImpl::SetFontProvider(f2dFontProvider* pProvider)
{
	if(m_pProvider == pProvider)
		return FCYERR_OK;

	FCYSAFEKILL(m_pProvider);
	m_pProvider = pProvider;
	if (m_pProvider)
		m_pProvider->AddRef();

	return FCYERR_OK;
}

fFloat f2dFontRendererImpl::GetZ()
{
	return m_ZValue;
}

void f2dFontRendererImpl::SetZ(fFloat ZValue)
{
	m_ZValue = ZValue;
}

fcyColor f2dFontRendererImpl::GetColor(fuInt Index)
{
	if(Index >= 4)
		return fcyColor();
	else
		return m_BlendColor[Index];
}

void f2dFontRendererImpl::SetColor(const fcyColor& Color)
{
	m_BlendColor[0] = Color;
	m_BlendColor[1] = Color;
	m_BlendColor[2] = Color;
	m_BlendColor[3] = Color;
}

void f2dFontRendererImpl::SetColor(fuInt Index, const fcyColor& Color)
{
	if(Index < 4)
		m_BlendColor[Index] = Color;
}

F2DSPRITEFLIP f2dFontRendererImpl::GetFlipType()
{
	return m_FlipType;
}

void f2dFontRendererImpl::SetFlipType(F2DSPRITEFLIP Type)
{
	m_FlipType = Type;
}

fcyVec2 f2dFontRendererImpl::GetScale()
{
	return m_Scale;
}

void f2dFontRendererImpl::SetScale(fcyVec2 Scale)
{
	m_Scale = Scale;
}

fcyRect f2dFontRendererImpl::MeasureString(fcStrW String, bool bStrictWidth)
{
	if (!m_pProvider)
		return fcyRect();

	fcyVec2 tStartPos;
	fcyRect tBoundBox(2048.f, 2048.f, 0.f, 0.f);
	bool tMeasureable = false;

	fuInt tCount = wcslen(String);

	f2dGlyphInfo tInfo;
	for(fuInt i = 0; i<tCount; ++i)
	{
		if(String[i] == L'\n')
		{
			tStartPos.x = 0;
			tStartPos.y += m_pProvider->GetLineHeight();
		}
		else
		{
			if(FCYOK(m_pProvider->QueryGlyph(NULL, String[i], &tInfo)))
			{
				tMeasureable = true;

				float tTop = tStartPos.y - tInfo.BrushPos.y;
				float tBottom = tStartPos.y + (tInfo.GlyphSize.y - tInfo.BrushPos.y);
				float tLeft = tStartPos.x - tInfo.BrushPos.x;
				float tWidth;
				if (bStrictWidth)
				{
					tWidth = tInfo.GlyphSize.x - tInfo.BrushPos.x;
					if (i + 1 < tCount && String[i + 1] != L'\n')
					{
						if (tWidth < tInfo.Advance.x)
							tWidth = tInfo.Advance.x;
					}
				}
				else
					tWidth = tInfo.Advance.x - tInfo.BrushPos.x;
				float tRight = tStartPos.x + tWidth;

				tBoundBox.a.x = FCYMIN(tBoundBox.a.x, tLeft);
				tBoundBox.b.x = FCYMAX(tBoundBox.b.x, tRight);
				tBoundBox.a.y = FCYMIN(tBoundBox.a.y, tTop);
				tBoundBox.b.y = FCYMAX(tBoundBox.b.y, tBottom);

				tStartPos += tInfo.Advance;
			}
		}
	}

	if (tMeasureable)
	{
		tBoundBox.b.x = tBoundBox.a.x + tBoundBox.GetWidth() * m_Scale.x;
		tBoundBox.b.y = tBoundBox.a.y + tBoundBox.GetHeight() * m_Scale.y;
		return tBoundBox;
	}
	else
		return fcyRect();
}

fFloat f2dFontRendererImpl::MeasureStringWidth(fcStrW String)
{
	if (!m_pProvider)
		return 0.f;

	fcyVec2 tStartPos;
	
	fuInt tCount = wcslen(String);

	f2dGlyphInfo tInfo;
	for(fuInt i = 0; i < tCount; ++i)
	{
		if(String[i] == L'\n')
			tStartPos.x = 0;
		else
		{
			if (FCYOK(m_pProvider->QueryGlyph(NULL, String[i], &tInfo)))
			{
				if (i + 1 < tCount && String[i + 1] != L'\n')
					tStartPos.x += tInfo.Advance.x;
				else
					tStartPos.x += tInfo.GlyphSize.x;
			}
		}
	}

	return tStartPos.x * m_Scale.x;
}

fResult f2dFontRendererImpl::DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos)
{
	return DrawTextW(pGraph, Text, -1, StartPos, NULL);
}

fResult f2dFontRendererImpl::DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut)
{
	if(!m_pProvider || !pGraph || !pGraph->IsInRender())
		return FCYERR_ILLEGAL;

	// --- ׼������ ---
	f2dGraphics2DVertex tVerts[4] = 
	{
		{ 0.f, 0.f, m_ZValue, m_BlendColor[0].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[1].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[2].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[3].argb, 0.f, 0.f }
	};

	// --- ������Ҫ���Ƶ����� ---
	fuInt tCount = wcslen(Text);                    // �ַ�����
	if(Count != -1)
		tCount = tCount < Count ? tCount : Count;

	fcyVec2 tPos = StartPos;  // �ʴ�λ��
	float tHeight = m_pProvider->GetLineHeight() * m_Scale.y;  // �и�

	// --- ����ÿһ���ַ� ---
	f2dTexture2D* pTex = m_pProvider->GetCacheTexture();
	if(!pTex)
		return FCYERR_INTERNALERR;

	f2dGlyphInfo tInfo;
	for(fuInt i = 0; i<tCount; ++i)
	{
		// ���д���
		if(Text[i] == L'\n')
		{
			tPos.x = StartPos.x;
			tPos.y += tHeight;
			continue;
		}

		// ȡ������
		if(FCYOK(m_pProvider->QueryGlyph(pGraph, Text[i], &tInfo)))
		{
			tInfo.Advance.x *= m_Scale.x;
			tInfo.Advance.y *= m_Scale.y;
			tInfo.BrushPos.x *= m_Scale.x;
			tInfo.BrushPos.y *= m_Scale.y;
			tInfo.GlyphSize.x *= m_Scale.x;
			tInfo.GlyphSize.y *= m_Scale.y;

			fBool tDraw;
			if(m_pListener)
				tDraw = m_pListener->OnGlyphBeginDraw(i, Text[i], tPos, tInfo.Advance);
			else
				tDraw = true;

			if(tDraw)
			{
				// ������ͼuv��Ϣ�����з�ת����
				copyAndFlipUV(tInfo, tVerts);

				// ����λ�þ���
				tVerts[0].x = tPos.x - tInfo.BrushPos.x;
				tVerts[0].y = tPos.y - tInfo.BrushPos.y;
				tVerts[1].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[1].y = tVerts[0].y;
				tVerts[2].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[2].y = tVerts[0].y + tInfo.GlyphSize.y;
				tVerts[3].x = tVerts[0].x;
				tVerts[3].y = tVerts[2].y;

				if(m_pListener)
					m_pListener->OnGlyphCalcuCoord(tVerts);

				// ��ͼ
				pGraph->DrawQuad(pTex, tVerts);
			}

			// �ʴ�λ�ú���
			tPos += tInfo.Advance;
		}
	}

	// �����µ�λ��
	if(PosOut)
		*PosOut = tPos;

	return FCYERR_OK;
}

fResult f2dFontRendererImpl::DrawTextW(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut)
{
	if (!m_pProvider || !pGraph || !pGraph->IsInRender())
		return FCYERR_ILLEGAL;

	// --- ׼������ ---
	f2dGraphics2DVertex tVerts[4] = 
	{
		{ 0.f, 0.f, m_ZValue, m_BlendColor[0].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[1].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[2].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[3].argb, 0.f, 0.f }
	};

	// --- ������Ҫ���Ƶ����� ---
	fuInt tCount = wcslen(Text);                    // �ַ�����
	if(Count != -1)
		tCount = tCount < Count ? tCount : Count;

	fcyVec2 tPos = StartPos;  // �ʴ�λ��
	float tHeight = m_pProvider->GetLineHeight() * m_Scale.y;  // �и�

	// Bias����
	Bias = tan(Bias);

	// --- ����ÿһ���ַ� ---
	f2dTexture2D* pTex = m_pProvider->GetCacheTexture();
	if(!pTex)
		return FCYERR_INTERNALERR;

	f2dGlyphInfo tInfo;
	for(fuInt i = 0; i<tCount; ++i)
	{
		// ���д���
		if(Text[i] == L'\n')
		{
			tPos.x = StartPos.x;
			tPos.y += tHeight;
			continue;
		}

		// ȡ������
		if(FCYOK(m_pProvider->QueryGlyph(pGraph, Text[i], &tInfo)))
		{
			tInfo.Advance.x *= m_Scale.x;
			tInfo.Advance.y *= m_Scale.y;
			tInfo.BrushPos.x *= m_Scale.x;
			tInfo.BrushPos.y *= m_Scale.y;
			tInfo.GlyphSize.x *= m_Scale.x;
			tInfo.GlyphSize.y *= m_Scale.y;

			fBool tDraw;
			if(m_pListener)
				tDraw = m_pListener->OnGlyphBeginDraw(i, Text[i], tPos, tInfo.Advance);
			else
				tDraw = true;

			if(tDraw)
			{
				// ������ͼuv��Ϣ�����з�ת����
				copyAndFlipUV(tInfo, tVerts);

				// ����λ�þ���
				float tBias = Bias * tInfo.GlyphSize.y;

				tVerts[0].x = tPos.x - tInfo.BrushPos.x;
				tVerts[0].y = tPos.y - tInfo.BrushPos.y;
				tVerts[1].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[1].y = tVerts[0].y;
				tVerts[2].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[2].y = tVerts[0].y + tInfo.GlyphSize.y;
				tVerts[3].x = tVerts[0].x;
				tVerts[3].y = tVerts[2].y;

				tVerts[0].x += tBias;
				tVerts[1].x += tBias;

				if(m_pListener)
					m_pListener->OnGlyphCalcuCoord(tVerts);

				// ��ͼ
				pGraph->DrawQuad(pTex, tVerts);
			}

			// �ʴ�λ�ú���
			tPos += tInfo.Advance;
		}
	}

	// �����µ�λ��
	if(PosOut)
		*PosOut = tPos;

	return FCYERR_OK;
}

fResult f2dFontRendererImpl::DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, const fcyVec2& StartPos)
{
	return DrawTextW2(pGraph, Text, -1, StartPos, NULL);
}

fResult f2dFontRendererImpl::DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, const fcyVec2& StartPos, fcyVec2* PosOut)
{
	if (!m_pProvider || !pGraph || !pGraph->IsInRender())
		return FCYERR_ILLEGAL;

	// --- ׼������ ---
	f2dGraphics2DVertex tVerts[4] =
	{
		{ 0.f, 0.f, m_ZValue, m_BlendColor[0].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[1].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[2].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[3].argb, 0.f, 0.f }
	};

	// --- ������Ҫ���Ƶ����� ---
	fuInt tCount = wcslen(Text);                    // �ַ�����
	if (Count != -1)
		tCount = tCount < Count ? tCount : Count;

	fcyVec2 tPos = StartPos;  // �ʴ�λ��
	float tHeight = m_pProvider->GetLineHeight() * m_Scale.y;  // �и�

	// --- ����ÿһ���ַ� ---
	f2dTexture2D* pTex = m_pProvider->GetCacheTexture();
	if (!pTex)
		return FCYERR_INTERNALERR;

	f2dGlyphInfo tInfo;
	for (fuInt i = 0; i<tCount; ++i)
	{
		// ���д���
		if (Text[i] == L'\n')
		{
			tPos.x = StartPos.x;
			tPos.y -= tHeight;
			continue;
		}

		// ȡ������
		if (FCYOK(m_pProvider->QueryGlyph(pGraph, Text[i], &tInfo)))
		{
			tInfo.Advance.x *= m_Scale.x;
			tInfo.Advance.y *= -m_Scale.y;
			tInfo.BrushPos.x *= m_Scale.x;
			tInfo.BrushPos.y *= -m_Scale.y;
			tInfo.GlyphSize.x *= m_Scale.x;
			tInfo.GlyphSize.y *= -m_Scale.y;

			fBool tDraw;
			if (m_pListener)
				tDraw = m_pListener->OnGlyphBeginDraw(i, Text[i], tPos, tInfo.Advance);
			else
				tDraw = true;

			if (tDraw)
			{
				// ������ͼuv��Ϣ�����з�ת����
				copyAndFlipUV(tInfo, tVerts);

				// ����λ�þ���
				tVerts[0].x = tPos.x - tInfo.BrushPos.x;
				tVerts[0].y = tPos.y - tInfo.BrushPos.y;
				tVerts[1].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[1].y = tVerts[0].y;
				tVerts[2].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[2].y = tVerts[0].y + tInfo.GlyphSize.y;
				tVerts[3].x = tVerts[0].x;
				tVerts[3].y = tVerts[2].y;

				if (m_pListener)
					m_pListener->OnGlyphCalcuCoord(tVerts);

				// ��ͼ
				pGraph->DrawQuad(pTex, tVerts);
			}

			// �ʴ�λ�ú���
			tPos += tInfo.Advance;
		}
	}

	// �����µ�λ��
	if (PosOut)
		*PosOut = tPos;

	return FCYERR_OK;
}

fResult f2dFontRendererImpl::DrawTextW2(f2dGraphics2D* pGraph, fcStrW Text, fuInt Count, fFloat Bias, const fcyVec2& StartPos, fcyVec2* PosOut)
{
	if (!m_pProvider || !pGraph || !pGraph->IsInRender())
		return FCYERR_ILLEGAL;

	// --- ׼������ ---
	f2dGraphics2DVertex tVerts[4] =
	{
		{ 0.f, 0.f, m_ZValue, m_BlendColor[0].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[1].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[2].argb, 0.f, 0.f },
		{ 0.f, 0.f, m_ZValue, m_BlendColor[3].argb, 0.f, 0.f }
	};

	// --- ������Ҫ���Ƶ����� ---
	fuInt tCount = wcslen(Text);                    // �ַ�����
	if (Count != -1)
		tCount = tCount < Count ? tCount : Count;

	fcyVec2 tPos = StartPos;  // �ʴ�λ��
	float tHeight = m_pProvider->GetLineHeight() * m_Scale.y;  // �и�

	// Bias����
	Bias = tan(Bias);

	// --- ����ÿһ���ַ� ---
	f2dTexture2D* pTex = m_pProvider->GetCacheTexture();
	if (!pTex)
		return FCYERR_INTERNALERR;

	f2dGlyphInfo tInfo;
	for (fuInt i = 0; i<tCount; ++i)
	{
		// ���д���
		if (Text[i] == L'\n')
		{
			tPos.x = StartPos.x;
			tPos.y -= tHeight;
			continue;
		}

		// ȡ������
		if (FCYOK(m_pProvider->QueryGlyph(pGraph, Text[i], &tInfo)))
		{
			tInfo.Advance.x *= m_Scale.x;
			tInfo.Advance.y *= -m_Scale.y;
			tInfo.BrushPos.x *= m_Scale.x;
			tInfo.BrushPos.y *= -m_Scale.y;
			tInfo.GlyphSize.x *= m_Scale.x;
			tInfo.GlyphSize.y *= -m_Scale.y;

			fBool tDraw;
			if (m_pListener)
				tDraw = m_pListener->OnGlyphBeginDraw(i, Text[i], tPos, tInfo.Advance);
			else
				tDraw = true;

			if (tDraw)
			{
				// ������ͼuv��Ϣ�����з�ת����
				copyAndFlipUV(tInfo, tVerts);

				// ����λ�þ���
				float tBias = Bias * tInfo.GlyphSize.y;

				tVerts[0].x = tPos.x - tInfo.BrushPos.x;
				tVerts[0].y = tPos.y - tInfo.BrushPos.y;
				tVerts[1].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[1].y = tVerts[0].y;
				tVerts[2].x = tVerts[0].x + tInfo.GlyphSize.x;
				tVerts[2].y = tVerts[0].y + tInfo.GlyphSize.y;
				tVerts[3].x = tVerts[0].x;
				tVerts[3].y = tVerts[2].y;

				tVerts[0].x += tBias;
				tVerts[1].x += tBias;

				if (m_pListener)
					m_pListener->OnGlyphCalcuCoord(tVerts);

				// ��ͼ
				pGraph->DrawQuad(pTex, tVerts);
			}

			// �ʴ�λ�ú���
			tPos += tInfo.Advance;
		}
	}

	// �����µ�λ��
	if (PosOut)
		*PosOut = tPos;

	return FCYERR_OK;
}
