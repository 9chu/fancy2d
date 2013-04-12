#pragma once

#include "fcyRefObj.h"
#include "f2d.h"

#include <stack>

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI渲染器
////////////////////////////////////////////////////////////////////////////////
class fuiRenderer :
	public fcyRefObjImpl<fcyRefObj>
{
private:
	f2dRenderer* m_pRenderer;
	f2dGraphics2D* m_pGraph;
	f2dGeometryRenderer* m_pGeometryRenderer;

	fcyMatrix4 m_OrgMat;
	fcyMatrix4 m_TransMat;

	fcyRect m_OrgClip;
	std::stack<fcyRect> m_ClipRect;
public:
	void Begin();
	void End();
	f2dGraphics2D* GetGraph();
	f2dGeometryRenderer* GetGeometryRenderer();
	fcyVec2* GetOffset();
	void ApplyOffset(const fcyVec2& Offset);
	fBool PushClip(const fcyRect& Rect); // 若裁剪矩阵不相交，返回假，并不推入矩形
	void PopClip();
public:
	fuiRenderer(f2dRenderer* pRenderer, f2dGraphics2D* pGraph);
protected:
	~fuiRenderer(void);
};
