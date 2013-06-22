#include "fuiControl.h"

#include <algorithm>

#include "fuiPage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fuiControl::fuiControl(fuiPage* pRootControl, const std::wstring& Name)
	: m_pRootPage(pRootControl), m_pStyle(NULL), m_pParent(NULL), m_Name(Name), 
	m_bClip(false), m_bMouseTrans(false), m_LayerPriority(0.f), m_bVisiable(true)
{
	if(m_pRootPage == NULL)
		throw fcyException("fuiControl::fuiControl", "Param 'pRootControl' is null.");
	m_pRootPage->RegisterControl(this);

	registerAllEvent();
	registerAllProperty();
}

fuiControl::~fuiControl()
{
	m_pRootPage->UnregisterControl(this);
}

void fuiControl::registerAllProperty()
{
	// === 设置默认访问器 ===
	// 名称
	m_Name_Accessor = fuiPropertyAccessor<std::wstring>(
		&m_Name, 
		fuiPropertyAccessorHelper<std::wstring>::DefaultGetter, // 可读
		fuiPropertyAccessor<std::wstring>::PropSetter()         // 拒绝写
	);
	// 控件位置
	m_Rect_Accessor = fuiPropertyAccessor<fcyRect>(
		&m_Rect,
		fuiPropertyAccessorHelper<fcyRect>::DefaultGetter,
		[&](const std::wstring& Prop, fcyRect*) {
			fcyRect tRect;
			fuiPropertyAccessorHelper<fcyRect>::DefaultSetter(Prop, &tRect);
			SetRect(tRect);
		}
	);
	// 左边位置
	m_Left_Accessor = fuiPropertyAccessor<float>(
		&m_Rect.a.x,
		fuiPropertyAccessorHelper<float>::DefaultGetter,
		[&](const std::wstring& Prop, float*) {
			float tValue;
			fuiPropertyAccessorHelper<float>::DefaultSetter(Prop, &tValue);
			SetLeft(tValue);
		}
	);
	// 顶边位置
	m_Top_Accessor = fuiPropertyAccessor<float>(
		&m_Rect.a.y,
		fuiPropertyAccessorHelper<float>::DefaultGetter,
		[&](const std::wstring& Prop, float*) {
			float tValue;
			fuiPropertyAccessorHelper<float>::DefaultSetter(Prop, &tValue);
			SetTop(tValue);
		}
	);
	// 宽度
	m_Width_Accessor = fuiPropertyAccessor<float>(
		NULL, 
		[&](std::wstring& Prop, const float*) {
			float v = GetWidth();
			fuiPropertyAccessorHelper<float>::DefaultGetter(Prop, &v);
		},
		[&](const std::wstring& Prop, float*) {
			float tValue;
			fuiPropertyAccessorHelper<float>::DefaultSetter(Prop, &tValue);
			SetWidth(tValue);
		}
	);
	// 高度
	m_Height_Accessor = fuiPropertyAccessor<float>(
		NULL, 
		[&](std::wstring& Prop, const float*) {
			float v = m_Rect.GetHeight();
			fuiPropertyAccessorHelper<float>::DefaultGetter(Prop, &v);
		},
		[&](const std::wstring& Prop, float*) {
			float tValue;
			fuiPropertyAccessorHelper<float>::DefaultSetter(Prop, &tValue);
			SetHeight(tValue);
		}
	);
	// 裁剪
	m_Clip_Accessor = fuiPropertyAccessor<bool>(&m_bClip);
	// 穿透
	m_MouseTrans_Accessor = fuiPropertyAccessor<bool>(&m_bMouseTrans);
	// 层次优先级
	m_LayerPriority_Accessor = fuiPropertyAccessor<fFloat>(
		&m_LayerPriority,
		[&](std::wstring& Prop, const float* v) {
			fuiPropertyAccessorHelper<float>::DefaultGetter(Prop, v);
		},
		[&](const std::wstring& Prop, float*) {
			float v;
			fuiPropertyAccessorHelper<float>::DefaultSetter(Prop, &v);
			SetLayerPriority(v);
		}
	);
	m_bVisiable_Accessor = fuiPropertyAccessor<bool>(&m_bVisiable);

	// 注册属性访问器
	RegisterProperty(L"Name", &m_Name_Accessor);
	RegisterProperty(L"Rect", &m_Rect_Accessor);
	RegisterProperty(L"Left", &m_Left_Accessor);
	RegisterProperty(L"Top", &m_Top_Accessor);
	RegisterProperty(L"Width", &m_Width_Accessor);
	RegisterProperty(L"Height", &m_Height_Accessor);
	RegisterProperty(L"Clip", &m_Clip_Accessor);
	RegisterProperty(L"MouseTrans", &m_MouseTrans_Accessor);
	RegisterProperty(L"LayerPriority", &m_LayerPriority_Accessor);
	RegisterProperty(L"Visiable", &m_bVisiable_Accessor);
}

void fuiControl::registerAllEvent()
{
	// 注册事件
	RegisterEvent(L"OnParentChanged");
	RegisterEvent(L"OnStyleChanged");
	RegisterEvent(L"OnPosChanged");
	RegisterEvent(L"OnSizeChanged");

	RegisterEvent(L"OnMouseMove");
	RegisterEvent(L"OnMouseLeave");
	RegisterEvent(L"OnMouseLDown");
	RegisterEvent(L"OnMouseLUp");
	RegisterEvent(L"OnMouseMDown");
	RegisterEvent(L"OnMouseMUp");
	RegisterEvent(L"OnMouseRDown");
	RegisterEvent(L"OnMouseRUp");

	RegisterEvent(L"OnSubcontrolLostFocus");
	RegisterEvent(L"OnSubcontrolGetFocus");
	
	RegisterEvent(L"OnLostFocus");
	RegisterEvent(L"OnGetFocus");

	RegisterEvent(L"OnKeyUp");
	RegisterEvent(L"OnKeyDown");

	RegisterEvent(L"OnMouseWheel");
	RegisterEvent(L"OnCharInput");
}

fuiControl* fuiControl::attachSubControl(fuiControl* pControl)
{
	if(!pControl)
		throw fcyException("fuiControl::attachSubControl", "Param 'pControl' is null.");

	if(pControl->GetParent() == this)
		return this;
	else
	{
		m_SubControlList.push_back(pControl);
		resortControl();
		return this;
	}
}

void fuiControl::removeSubControl(fuiControl* pControl)
{
	if(!pControl)
		throw fcyException("fuiControl::attachSubControl", "Param 'pControl' is null.");

	vector<fcyRefPointer<fuiControl>>::iterator i = m_SubControlList.begin();
	while(i != m_SubControlList.end())
	{
		if(*(*i) == pControl)
		{
			m_SubControlList.erase(i);
			return;
		}
		else
			i++;
	}
}

fuInt fuiControl::getSubControlLayer(const fuiControl* pControl)
{
	if(!pControl)
		throw fcyException("fuiControl::attachSubControl", "Param 'pControl' is null.");

	for(fuInt i = 0; i<m_SubControlList.size(); i++)
	{
		if(*m_SubControlList[i] == pControl)
			return i;
	}

	throw fcyException("fuiControl::getSubControlLayer", "Object not found.");
}

void fuiControl::setSubControlLayer(fuiControl* pControl, fInt Index)
{
	if(!pControl)
		throw fcyException("fuiControl::attachSubControl", "Param 'pControl' is null.");

	// 确认索引
	if(Index < 0 && -(Index + 1) > (int)m_SubControlList.size())
		throw fcyException("fuiControl::setSubControlLayer", "Index out of range.");
	else if(Index > 0 && Index > (int)m_SubControlList.size())
		throw fcyException("fuiControl::setSubControlLayer", "Index out of range.");
	
	pControl->AddRef();

	removeSubControl(pControl);

	if(Index >= 0)
		m_SubControlList.insert(m_SubControlList.begin() + Index, pControl);
	else
		m_SubControlList.insert((m_SubControlList.rbegin() + (-(Index + 1))).base(), pControl);

	pControl->Release();

	resortControl();
}

void fuiControl::resortControl()
{
	struct {
		bool operator()(const fcyRefPointer<fuiControl>& p1, const fcyRefPointer<fuiControl>& p2)
		{
			return p1->GetLayerPriority() < p2->GetLayerPriority();
		}
	} _SortOperator;

	sort(m_SubControlList.begin(), m_SubControlList.end(), _SortOperator);
}

fcyVec2 fuiControl::GetAbsolutePos()
{
	fcyVec2 tRet = GetRect().a;
	fuiControl* p = this;

	while(p->GetParent() && p->GetParent() != this)
	{
		tRet += p->GetParent()->GetRect().a;
		p = p->GetParent();
	}

	return tRet;
}

void fuiControl::SetParent(fuiControl* pParent)
{
	if(pParent == m_pParent)
		return;

	// 从父对象中移除
	if(m_pParent)
		m_pParent->removeSubControl(this);

	// 挂接到新的对象
	if(pParent)
		m_pParent = pParent->attachSubControl(this);
	else
		pParent = NULL;

	// 触发事件
	ExecEvent(L"OnParentChanged");
}

fuInt fuiControl::GetLayer()const
{
	if(m_pParent)
		return m_pParent->getSubControlLayer(this);
	return 0;
}

void fuiControl::SetLayer(fInt Layer)
{
	if(m_pParent)
		m_pParent->setSubControlLayer(this, Layer);
}

fuInt fuiControl::GetSubControlCount()const
{
	return m_SubControlList.size();
}

fuiControl* fuiControl::GetSubControl(fInt Layer)
{
	// 确认索引
	if(Layer < 0 && -(Layer + 1) > (int)m_SubControlList.size())
		throw fcyException("fuiControl::GetSubControl", "Index out of range.");
	else if(Layer > 0 && Layer > (int)m_SubControlList.size())
		throw fcyException("fuiControl::GetSubControl", "Index out of range.");
	
	if(Layer >= 0)
		return m_SubControlList[Layer];
	else
		return *((m_SubControlList.rbegin() + (-(Layer + 1))).base());
}

fuiStyle* fuiControl::GetControlStyle()const
{
	if(m_pStyle)
		return m_pStyle;
	else
		return m_pRootPage->GetControlStyle();
}

void fuiControl::SetControlStyle(fuiStyle* pStyle)
{
	if(m_pStyle == pStyle)
		return;

	m_pStyle = pStyle;

	// 触发事件
	ExecEvent(L"OnStyleChanged");
}

void fuiControl::SetRect(const fcyRect& Rect)
{
	if(Rect.b.x < Rect.a.x || Rect.b.y < Rect.a.y)
		throw fcyException("fuiControl::SetRect", "Invalid value.");
	
	m_Rect = Rect;

	// 触发事件
	ExecEvent(L"OnPosChanged");
	ExecEvent(L"OnSizeChanged");
}

void fuiControl::SetLeft(fFloat Value)
{
	float tOrgWidth = m_Rect.GetWidth();
	m_Rect.a.x = Value;
	m_Rect.b.x = m_Rect.a.x + tOrgWidth;

	// 触发事件
	ExecEvent(L"OnPosChanged");
}

void fuiControl::SetTop(fFloat Value)
{
	float tOrgHeight = m_Rect.GetHeight();
	m_Rect.a.y = Value;
	m_Rect.b.y = m_Rect.a.y + tOrgHeight;

	// 触发事件
	ExecEvent(L"OnPosChanged");
}

void fuiControl::SetWidth(fFloat Value)
{
	if(Value < 0)
		throw fcyException("fuiControl::SetWidth", "Invalid value.");

	m_Rect.b.x = m_Rect.a.x + Value;

	// 触发事件
	ExecEvent(L"OnSizeChanged");
}

void fuiControl::SetHeight(fFloat Value)
{
	if(Value < 0)
		throw fcyException("fuiControl::SetHeight", "Invalid value.");

	m_Rect.b.y = m_Rect.a.y + Value;

	// 触发事件
	ExecEvent(L"OnSizeChanged");
}

void fuiControl::SetLayerPriority(fFloat Value)
{
	m_LayerPriority = Value;
	if(m_pParent && m_pParent != this)
		m_pParent->resortControl();
}

void fuiControl::Update(fDouble ElapsedTime)
{
	for(fuInt i = 0; i<m_SubControlList.size(); i++)
	{
		m_SubControlList[i]->Update(ElapsedTime);
	}
}

void fuiControl::Render(fuiGraphics* pGraph)
{
	for(fuInt i = 0; i<m_SubControlList.size(); i++)
	{
		fuiControl* p = m_SubControlList[i];

		if(p->GetVisiable())
		{
			if(p->GetClip())
				pGraph->PushClipRect(p->m_Rect);
			pGraph->PushOffset(p->m_Rect.a);

			p->Render(pGraph);

			if(p->GetClip())
				pGraph->PopClipRect();
			pGraph->PopOffset();
		}
	}
}
