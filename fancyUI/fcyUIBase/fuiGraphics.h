////////////////////////////////////////////////////////////////////////////////
/// @file  fuiGraphics.h
/// @brief fancyUI »æÍ¼Æ÷
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyIO/fcyStream.h>
#include <fcyException.h>

#include <f2d.h>

#include <stack>

////////////////////////////////////////////////////////////////////////////////
/// @brief »æÍ¼Æ÷
////////////////////////////////////////////////////////////////////////////////
class fuiGraphics
{
protected:
	f2dRenderDevice* m_pDev;
	f2dGraphics2D* m_pGraphics2D;
	fcyRefPointer<f2dGeometryRenderer> m_pGeometry;

	fBool m_InRender;

	fcyMatrix4 m_TransMat;
	fcyMatrix4 m_OrgTransMat;

	std::stack<fcyRect> m_ClipRect;
	std::stack<fcyVec2> m_Offsets;
public:
	f2dRenderDevice* GetDevice() { return m_pDev; }
	f2dGraphics2D* GetGraphics() { return m_pGraphics2D; }
	f2dGeometryRenderer* GetGeometry() { return m_pGeometry; }

	void PushClipRect(const fcyRect& Rect);
	void PopClipRect();
	void PushOffset(const fcyVec2& Offset);
	void PopOffset();
	fcyVec2 GetTopOffset();
	void DisableOffset();
	void RestoreOffset();

	fResult Begin();
	void End();
public:
	fuiGraphics(f2dRenderer* pRenderer, f2dGraphics2D* pGraph);
	~fuiGraphics();
};
