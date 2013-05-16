#include "f2dRendererImpl.h"

#include "f2dSpriteImpl.h"
#include "f2dGeometryRendererImpl.h"

#include "../Engine/f2dEngineImpl.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

f2dRendererImpl::f2dRendererImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
	: m_pEngine(pEngine), m_pDev(NULL)
{
	m_pDev = new f2dRenderDeviceImpl(pEngine, BackBufferWidth, BackBufferHeight, Windowed, VSync, AALevel);
}

f2dRendererImpl::~f2dRendererImpl()
{
	FCYSAFEKILL(m_pDev);
}

f2dRenderDevice* f2dRendererImpl::GetDevice()
{
	return m_pDev;
}

fResult f2dRendererImpl::CreateSprite2D(f2dTexture2D* pTex, f2dSprite** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pTex)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dSpriteImpl(pTex);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateSprite2D(f2dTexture2D* pTex, const fcyRect& Org, f2dSprite** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pTex)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dSpriteImpl(pTex, Org);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateSprite2D(f2dTexture2D* pTex, const fcyRect& Org, const fcyVec2& HotSopt, f2dSprite** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pTex)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dSpriteImpl(pTex, Org, HotSopt);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateGeometryRenderer(f2dGeometryRenderer** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dGeometryRendererImpl();
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateFontRenderer(f2dFontProvider* pProvider, f2dFontRenderer** pOut)
{
	return FCYERR_NOTIMPL;
}

fResult f2dRendererImpl::CreateFontFromFile(f2dStream* pStream, fuInt FaceIndex, F2DFONTFLAG Flag, f2dFontProvider** pOut)
{
	return FCYERR_NOTIMPL;
}

fResult f2dRendererImpl::CreateFontFromTex(f2dStream* pDefineFile, f2dTexture2D* pTex, f2dFontProvider** pOut)
{
	return FCYERR_NOTIMPL;
}
