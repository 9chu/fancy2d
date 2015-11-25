////////////////////////////////////////////////////////////////////////////////
/// @file  f2dSpriteImpl.h
/// @brief fancy2D ����ʵ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include "../f2dRenderer.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D ����ʵ��
////////////////////////////////////////////////////////////////////////////////
class f2dSpriteImpl : 
	public fcyRefObjImpl<f2dSprite>
{
	friend class f2dRendererImpl;
protected:
	f2dTexture2D* m_pTex;
	F2DSPRITEFLIP m_FlipType;
	fcyRect m_Org;
	fcyVec2 m_HotSpot;
	fcyColor m_Color[4];
	float m_ZValue;

	fcyRect m_UV;
	fcyRect m_SpriteRect;
protected:
	void recalcu();
public: // �ӿ�ʵ��
	f2dTexture2D* GetTexture();
	fResult SetTexture(f2dTexture2D* pTex);
	const fcyRect& GetTexRect()const;
	fResult SetTexRect(const fcyRect& Rect);
	const fcyVec2& GetHotSpot()const;
	fResult SetHotSpot(const fcyVec2& Point);
	F2DSPRITEFLIP GetFlipType()const;
	fResult SetFlipType(F2DSPRITEFLIP Type);
	fFloat GetZ()const;
	fResult SetZ(fFloat Value);
	fcyColor GetColor(fuInt Index)const;
	void GetColor(fcyColor* pOut)const;
	fResult SetColor(fcyColor Color);
	fResult SetColor(fuInt Index, fcyColor Color);
	fResult SetColor(fcyColor* pArr);
	fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, fBool bAutoFixCoord);
	fResult Draw(f2dGraphics2D* pGraph, const fcyRect& Dest, const fcyRect& SubTex);
	fResult Draw(f2dGraphics2D* pGraph, fcyVec3 v1, fcyVec3 v2, fcyVec3 v3, fcyVec3 v4, fBool bAutoFixCoord);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, fBool bAutoFixCoord);
	fResult Draw(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex);
	fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center);
	fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale);
	fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, const fcyRect& SubTex);
	fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, fBool bAutoFixCoord);
	fResult Draw2(f2dGraphics2D* pGraph, const fcyVec2& Center, const fcyVec2& Scale, fFloat Rotation, const fcyRect& SubTex);
public:
	f2dSpriteImpl(f2dTexture2D* pTex);
	f2dSpriteImpl(f2dTexture2D* pTex, fcyRect SrcRect);
	f2dSpriteImpl(f2dTexture2D* pTex, fcyRect SrcRect, fcyVec2 HotSpot);
	f2dSpriteImpl(const f2dSpriteImpl* pOrg);
	~f2dSpriteImpl();
};
