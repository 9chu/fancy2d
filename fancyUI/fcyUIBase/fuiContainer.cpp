#include "fuiContainer.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const fCharW fuiContainer::ControlName[] = L"Container";

fuiContainer::fuiContainer(fuiContainer* pParnet)
	: fuiControl(pParnet), m_pFocus(NULL), m_pMouseObj(NULL), m_pMouseCatch(NULL), m_MouseEvent(FUIMOUSEEVENT_MOVE)
{}

fuiContainer::~fuiContainer(void)
{
	clearControls();
}

void fuiContainer::updateMouseObj(fFloat X, fFloat Y)
{
	fcyVec2 MousePos;
	OnQuerySubControlOffset(MousePos);
	X -= MousePos.x;
	Y -= MousePos.y;

	m_pMouseObj = NULL;

	// 寻找对象
	// === 启用正向迭代器 ===
	list<fuiControl*>::iterator i = m_SubControl.begin();
	list<fuiControl*>::iterator ti;
	m_IterList.push_back(&i);

	while(i != m_SubControl.end())
	{
		ti = i;

		if((*i)->IsVisiable() && (*i)->GetRect().Contain(fcyVec2(X,Y)))
		{
			// 检查对象是否接受消息
			if((*i)->OnMouseMove(X - (*i)->GetRect().a.x, Y - (*i)->GetRect().a.y))
			{
				m_pMouseObj = (*i);
				m_IterList.pop_back();
				return;
			}
		}

		if(i == m_SubControl.end())
			break;
		else if((*i) == (*ti))
			i++;
	}

	// === 取消迭代器 ===
	m_IterList.pop_back();
}

void fuiContainer::appendControl(fuiControl* pControl)
{
	m_SubControl.push_front(pControl);
}

void fuiContainer::removeControl(fuiControl* pControl)
{
	// 遍历寻找到对象
	list<fuiControl*>::iterator i = m_SubControl.begin();
	while(i != m_SubControl.end())
	{
		if((*i) == pControl)
			break;
		else
			i++;
	}

	// 错误调用
	if(i == m_SubControl.end())
	{
#ifdef _DEBUG
		throw fcyException("fuiContainer::removeControl", "Remove a nonexistent obj.");
#endif
		return;
	}

	// 得到对象
	fuiControl* pObj = (*i);
	if(m_pFocus == pObj)
		m_pFocus = NULL;
	if(m_pMouseCatch == pObj)
	{
		m_MouseEvent = FUIMOUSEEVENT_MOVE;
		m_pMouseCatch = NULL;
	}
	if(m_pMouseObj == pObj)
		m_pMouseObj = NULL;

	// 修改迭代器
	vector<list<fuiControl*>::iterator*>::iterator j = m_IterList.begin();
	for(; j!=m_IterList.end(); j++)
	{
		if(*(*j) != m_SubControl.end() && **(*j) == pObj)
			(*(*j))++;
	}

	vector<list<fuiControl*>::reverse_iterator*>::iterator k = m_RevIterList.begin();
	for(; k!=m_RevIterList.end(); k++)
	{
		if(*(*k) != m_SubControl.rend() && **(*k) == pObj)
			(*(*k))++;
	}

	// 删除迭代器
	m_SubControl.erase(i);
}

fuInt fuiContainer::getControlIndex(fuiControl* pControl)
{
	fuInt tRet = 0;

	list<fuiControl*>::iterator i = m_SubControl.begin();
	while(i != m_SubControl.end())
	{
		if((*i) == pControl)
			break;
		else
		{
			i++;
			tRet++;
		}
	}
	
	// 错误调用
	if(i == m_SubControl.end())
	{
#ifdef _DEBUG
		throw fcyException("fuiContainer::getControlIndex", "Operate on a nonexistent obj.");
#endif
		return 0;
	}

	return tRet;
}

void fuiContainer::setControlIndex(fuiControl* pControl, fuInt Index)
{
	// 先移除对象
	{
		// 遍历寻找到对象
		list<fuiControl*>::iterator i = m_SubControl.begin();
		while(i != m_SubControl.end())
		{
			if((*i) == pControl)
				break;
			else
				i++;
		}

		// 错误调用
		if(i == m_SubControl.end())
		{
#ifdef _DEBUG
			throw fcyException("fuiContainer::setControlIndex", "Remove a nonexistent obj.");
#endif
			return;
		}

		// 得到对象
		fuiControl* pObj = (*i);

		// 修改迭代器
		vector<list<fuiControl*>::iterator*>::iterator j = m_IterList.begin();
		for(; j!=m_IterList.end(); j++)
		{
			if(*(*j) != m_SubControl.end() && **(*j) == pObj)
				(*(*j))++;
		}

		vector<list<fuiControl*>::reverse_iterator*>::iterator k = m_RevIterList.begin();
		for(; k!=m_RevIterList.end(); k++)
		{
			if(*(*k) != m_SubControl.rend() && **(*k) == pObj)
				(*(*k))++;
		}

		// 删除迭代器
		m_SubControl.erase(i);
	}

	if(Index > m_SubControl.size())
		Index = m_SubControl.size();

	list<fuiControl*>::iterator i = m_SubControl.begin();
	while(Index != 0)
	{
		i++;
		Index--;
	}

	// 插入对象
	m_SubControl.insert(i, pControl);
}

void fuiContainer::clearControls()
{
	// 清除所有控件
	list<fuiControl*>::iterator i = m_SubControl.begin();
	while(i != m_SubControl.end())
	{
		FCYSAFEDEL(*i);
		i = m_SubControl.erase(i);
	}

	// 重置状态
	m_pFocus = NULL;
	m_pMouseCatch = NULL;
	m_pMouseObj = NULL;
	m_MouseEvent = FUIMOUSEEVENT_MOVE;

	// 设置所有迭代器
	vector<list<fuiControl*>::iterator*>::iterator j = m_IterList.begin();
	for(; j!=m_IterList.end(); j++)
	{
		*(*j) = m_SubControl.end();
	}

	vector<list<fuiControl*>::reverse_iterator*>::iterator k = m_RevIterList.begin();
	for(; k!=m_RevIterList.end(); k++)
	{
		*(*k) = m_SubControl.rend();
	}
}

void fuiContainer::subControlLostFocus(fuiControl* pControl)
{
	if(m_pFocus == pControl)
	{
		m_pFocus = NULL;
		OnSubControlLostFocus(m_pFocus);
	}
}

void fuiContainer::subControlGetFocus(fuiControl* pControl)
{
	if(m_pFocus && m_pFocus != pControl)
		m_pFocus->OnLostFocus();
	m_pFocus = pControl;
	OnSubControlGetFocus(pControl);
	GetFocus();
}

void fuiContainer::subControlResized(fuiControl* pControl)
{
	OnSubControlResized(pControl);
}

////////////////////////////////////////////////////////////////////////////////

void fuiContainer::OnSubControlLostFocus(fuiControl* pControl)
{}

void fuiContainer::OnSubControlGetFocus(fuiControl* pControl)
{}

void fuiContainer::OnSubControlResized(fuiControl* pControl)
{}

////////////////////////////////////////////////////////////////////////////////

void fuiContainer::OnRender(fuiRenderer* pRenderer)
{
	/*
	f2dGeometryRenderer* p = pRenderer->GetGeometryRenderer();
	p->SetPenColor(0, 0xFFFF0000);
	p->SetPenColor(1, 0xFFFF0000);
	p->SetPenColor(2, 0xFFFF0000);
	p->SetPenSize(2.f);
	p->DrawRectangle(pRenderer->GetGraph(), GetRect());
	*/

	// === 位移子对象 ===
	fcyVec2 pOffset;
	OnQuerySubControlOffset(pOffset);
	pOffset += GetRect().a;
	pRenderer->ApplyOffset(pOffset);

	// === 启用反向迭代器 ===
	list<fuiControl*>::reverse_iterator i = m_SubControl.rbegin();
	list<fuiControl*>::reverse_iterator ti;
	m_RevIterList.push_back(&i);
	while(i != m_SubControl.rend())
	{
		ti = i;

		// 渲染子对象
		(*i)->Render(pRenderer);

		if(i == m_SubControl.rend())
			break;
		else if((*i) == (*ti))
			i++;
	}

	// === 取消迭代器 ===
	m_RevIterList.pop_back();

	// === 恢复位移 ===
	pRenderer->ApplyOffset(-pOffset);
}

void fuiContainer::OnUpdate(fDouble ElapsedTime)
{
	// === 启用反向迭代器 ===
	list<fuiControl*>::reverse_iterator i = m_SubControl.rbegin();
	list<fuiControl*>::reverse_iterator ti;
	m_RevIterList.push_back(&i);
	while(i != m_SubControl.rend())
	{
		ti = i;

		// 渲染子对象
		(*i)->Update(ElapsedTime);

		if(i == m_SubControl.rend())
			break;
		else if((*i) == (*ti))
			i++;
	}

	// === 取消迭代器 ===
	m_RevIterList.pop_back();
}

fBool fuiContainer::OnGetFocus()
{
	if(m_pFocus && m_pFocus!=this)
	{
		if(!m_pFocus->OnGetFocus())
			m_pFocus = NULL;
	}

	return true;
}

void fuiContainer::OnLostFocus()
{
	if(m_pFocus && m_pFocus!=this)
		m_pFocus->OnLostFocus();
}

void fuiContainer::OnMouseLeave()
{
	if(m_pMouseCatch)
	{
		// 如果有捕获对象，直接发送
		if(m_pMouseCatch == this)
			return;
		else
			return m_pMouseCatch->OnMouseLeave();
	}
	else
	{
		// 发送给当前位置对象
		if(m_pMouseObj)
			m_pMouseObj->OnMouseLeave();
	}
}

fBool fuiContainer::OnMouseMove(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	if(m_pMouseCatch)
	{
		// 如果有捕获对象，直接发送
		if(m_pMouseCatch == this)
			return false;
		else
		{
			fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
			tOffset += tMouseOffset;
			return m_pMouseCatch->OnMouseMove(X - tOffset.x, Y - tOffset.y);
		}
	}
	else
	{
		// 寻找当前位置
		fuiControl* pLast = m_pMouseObj;

		updateMouseObj(X,Y);

		if(pLast && pLast != m_pMouseObj)
			pLast->OnMouseLeave();

		return (m_pMouseObj != NULL);
	}
}

void fuiContainer::OnLMouseUp(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	// 该消息只发送给捕获对象
	if(m_pMouseCatch && (m_pMouseCatch!=this))
	{
		fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
		tOffset += tMouseOffset;
		m_pMouseCatch->OnLMouseUp(X - tOffset.x, Y - tOffset.y);
	}

	// 回收捕获焦点
	if(m_MouseEvent == FUIMOUSEEVENT_LDOWN)
	{
		m_MouseEvent = FUIMOUSEEVENT_MOVE;
		m_pMouseCatch = NULL;
	}
}

fBool fuiContainer::OnLMouseDown(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	if(m_pMouseCatch)
	{
		// 如果有捕获对象，直接发送
		if(m_pMouseCatch == this)
			return false;
		else
		{
			fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
			tOffset += tMouseOffset;
			return m_pMouseCatch->OnLMouseDown(X - tOffset.x, Y - tOffset.y);
		}
	}
	else
	{
		// 向当前位置投递一个MouseMove消息获得接受对象
		updateMouseObj(X, Y);

		// 有接受对象
		if(m_pMouseObj)
		{
			fcyVec2 tOffset = m_pMouseObj->GetRect().a;
			tOffset += tMouseOffset;

			if(m_pMouseObj->OnLMouseDown(X - tOffset.x, Y - tOffset.y))
			{
				m_pMouseCatch = m_pMouseObj;
				m_MouseEvent = FUIMOUSEEVENT_LDOWN;
				return true;
			}
		}
	}

	return false;
}

void fuiContainer::OnRMouseUp(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	// 该消息只发送给捕获对象
	if(m_pMouseCatch && (m_pMouseCatch!=this))
	{
		fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
		tOffset += tMouseOffset;
		m_pMouseCatch->OnLMouseUp(X - tOffset.x, Y - tOffset.y);
	}

	// 回收捕获焦点
	if(m_MouseEvent == FUIMOUSEEVENT_RDOWN)
	{
		m_MouseEvent = FUIMOUSEEVENT_MOVE;
		m_pMouseCatch = NULL;
	}
}

fBool fuiContainer::OnRMouseDown(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	if(m_pMouseCatch)
	{
		// 如果有捕获对象，直接发送
		if(m_pMouseCatch == this)
			return false;
		else
		{
			fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
			tOffset += tMouseOffset;
			return m_pMouseCatch->OnRMouseDown(X - tOffset.x, Y - tOffset.y);
		}
	}
	else
	{
		// 向当前位置投递一个MouseMove消息获得接受对象
		updateMouseObj(X, Y);

		// 有接受对象
		if(m_pMouseObj)
		{
			fcyVec2 tOffset = m_pMouseObj->GetRect().a;
			tOffset += tMouseOffset;
			if(m_pMouseObj->OnRMouseDown(X - tOffset.x, Y - tOffset.y))
			{
				m_pMouseCatch = m_pMouseObj;
				m_MouseEvent = FUIMOUSEEVENT_RDOWN;
				return true;
			}
		}
	}

	return false;
}

void fuiContainer::OnMMouseUp(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	// 该消息只发送给捕获对象
	if(m_pMouseCatch && (m_pMouseCatch!=this))
	{
		fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
		tOffset += tMouseOffset;
		m_pMouseCatch->OnLMouseUp(X - tOffset.x, Y - tOffset.y);
	}

	// 回收捕获焦点
	if(m_MouseEvent == FUIMOUSEEVENT_MDOWN)
	{
		m_MouseEvent = FUIMOUSEEVENT_MOVE;
		m_pMouseCatch = NULL;
	}
}

fBool fuiContainer::OnMMouseDown(fFloat X, fFloat Y)
{
	fcyVec2 tMouseOffset;
	OnQuerySubControlOffset(tMouseOffset);

	if(m_pMouseCatch)
	{
		// 如果有捕获对象，直接发送
		if(m_pMouseCatch == this)
			return false;
		else
		{
			fcyVec2 tOffset = m_pMouseCatch->GetRect().a;
			tOffset += tMouseOffset;
			return m_pMouseCatch->OnMMouseDown(X - tOffset.x, Y - tOffset.y);
		}
	}
	else
	{
		// 向当前位置投递一个MouseMove消息获得接受对象
		updateMouseObj(X, Y);

		// 有接受对象
		if(m_pMouseObj)
		{
			fcyVec2 tOffset = m_pMouseObj->GetRect().a;
			tOffset += tMouseOffset;

			if(m_pMouseObj->OnMMouseDown(X - tOffset.x, Y - tOffset.y))
			{
				m_pMouseCatch = m_pMouseObj;
				m_MouseEvent = FUIMOUSEEVENT_MDOWN;
				return true;
			}
		}
	}

	return false;
}

void fuiContainer::OnKeyUp(F2DINPUTKEYCODE KeyCode)
{
	if(m_pFocus && m_pFocus!=this)
		m_pFocus->OnKeyUp(KeyCode);
}

void fuiContainer::OnKeyDown(F2DINPUTKEYCODE KeyCode)
{
	if(m_pFocus && m_pFocus!=this)
		m_pFocus->OnKeyDown(KeyCode);
}

void fuiContainer::OnCharInput(fCharW CharCode)
{
	if(m_pFocus && m_pFocus!=this)
		m_pFocus->OnCharInput(CharCode);
}

void fuiContainer::OnQueryControlName(wstring& Out)
{
	fuiControl::OnQueryControlName(Out);

	Out += L",";
	Out += ControlName;
}
