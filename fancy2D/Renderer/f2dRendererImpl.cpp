#include "f2dRendererImpl.h"

#include "f2dSpriteImpl.h"
#include "f2dGeometryRendererImpl.h"
#include "f2dFontRendererImpl.h"
#include "f2dFontFileProvider.h"
#include "f2dFontTexProvider.h"
#include "f2dSpriteAnimationImpl.h"
#include "f2dParticle.h"

#include "../Engine/f2dEngineImpl.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

void f2dRendererImpl::DefaultListener::OnRenderDeviceLost()
{
	m_pEngine->SendMsg(F2DMSG_RENDER_ONDEVLOST);
}

void f2dRendererImpl::DefaultListener::OnRenderDeviceReset()
{
	m_pEngine->SendMsg(F2DMSG_RENDER_ONDEVRESET);
}

////////////////////////////////////////////////////////////////////////////////

f2dRendererImpl::f2dRendererImpl(f2dEngineImpl* pEngine, fuInt BackBufferWidth, fuInt BackBufferHeight, fBool Windowed, fBool VSync, F2DAALEVEL AALevel)
	: m_pEngine(pEngine), m_pDev(NULL), m_DefaultListener(pEngine)
{
	m_pDev = new f2dRenderDeviceImpl(pEngine, BackBufferWidth, BackBufferHeight, Windowed, VSync, AALevel);
	m_pDev->AttachListener(&m_DefaultListener);
}

f2dRendererImpl::~f2dRendererImpl()
{
	m_pDev->RemoveListener(&m_DefaultListener);
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
		return FCYERR_INTERNALERR;
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
		return FCYERR_INTERNALERR;
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
		return FCYERR_INTERNALERR;
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
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateFontRenderer(f2dFontProvider* pProvider, f2dFontRenderer** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dFontRendererImpl(pProvider);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateFontFromFile(f2dStream* pStream, fuInt FaceIndex, const fcyVec2& FontSize, F2DFONTFLAG Flag, f2dFontProvider** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pStream)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dFontFileProvider(m_pDev, pStream, FontSize, FaceIndex, Flag);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateFontFromTex(f2dStream* pDefineFile, f2dTexture2D* pTex, f2dFontProvider** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pDefineFile)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dFontTexProvider(pDefineFile, pTex);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateFontFromTex(fcStrW pDefineText, f2dTexture2D* pTex, f2dFontProvider** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	if(!pDefineText)
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dFontTexProvider(pDefineText, pTex);
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateSpriteAnimation(f2dSpriteAnimation** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dSpriteAnimationImpl();
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}

fResult f2dRendererImpl::CreateParticlePool(f2dParticlePool** pOut)
{
	if(pOut)
		*pOut = NULL;
	else
		return FCYERR_INVAILDPARAM;

	try
	{
		*pOut = new f2dParticlePoolImpl();
	}
	catch(const fcyException& e)
	{
		m_pEngine->ThrowException(e);
		return FCYERR_INTERNALERR;
	}

	return FCYERR_OK;
}
