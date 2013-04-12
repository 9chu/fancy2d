#pragma once
#include "fuiControl.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI容器
////////////////////////////////////////////////////////////////////////////////
class fuiContainer :
	public fuiControl
{
	friend class fuiControl;
private:
	static const fCharW ControlName[];
protected:
	// === 对象列表 ===
	std::list<fuiControl*> m_SubControl;                                  ///< @brief 子控件表，最开头为最顶层，Index=0
	std::vector<std::list<fuiControl*>::iterator*> m_IterList;            ///< @brief 正向迭代器
	std::vector<std::list<fuiControl*>::reverse_iterator*> m_RevIterList; ///< @brief 反向迭代器

	// === 消息控制 ===
	fuiControl* m_pFocus;       ///< @brief 当前焦点对象
	fuiControl* m_pMouseObj;    ///< @brief 当前鼠标位置处的对象
	fuiControl* m_pMouseCatch;  ///< @brief 当前正捕获鼠标消息的对象
	FUIMOUSEEVENT m_MouseEvent; ///< @brief 被捕获的鼠标消息
private:
	void updateMouseObj(fFloat X, fFloat Y);                 ///< @brief 更新鼠标处对象
protected: // 交由Control被动调用的内部方法
	void appendControl(fuiControl* pControl);                ///< @brief 追加子控件 
	void removeControl(fuiControl* pControl);                ///< @brief 移除子控件
	fuInt getControlIndex(fuiControl* pControl);             ///< @brief 获得控件索引
	void setControlIndex(fuiControl* pControl, fuInt Index); ///< @brief 设置控件索引
	void clearControls();                                    ///< @brief 清空所有控件
	void subControlLostFocus(fuiControl* pControl);          ///< @brief 子控件丢失焦点
	void subControlGetFocus(fuiControl* pControl);           ///< @brief 子控件获得焦点
	void subControlResized(fuiControl* pControl);            ///< @brief 子控件大小改变
protected: // 可覆写方法
	virtual void OnSubControlLostFocus(fuiControl* pControl);///< @brief 子对象丢失焦点
	virtual void OnSubControlGetFocus(fuiControl* pControl); ///< @brief 子对象丢失焦点
	virtual void OnSubControlResized(fuiControl* pControl);  ///< @brief 子对象大小重置
protected: // 覆写基类方法
	virtual void OnRender(fuiRenderer* pRenderer);
	virtual void OnUpdate(fDouble ElapsedTime);
	virtual fBool OnGetFocus();
	virtual void OnLostFocus();
	virtual void OnMouseLeave();
	virtual fBool OnMouseMove(fFloat X, fFloat Y);
	virtual void OnLMouseUp(fFloat X, fFloat Y);
	virtual fBool OnLMouseDown(fFloat X, fFloat Y);
	virtual void OnRMouseUp(fFloat X, fFloat Y);
	virtual fBool OnRMouseDown(fFloat X, fFloat Y);
	virtual void OnMMouseUp(fFloat X, fFloat Y);
	virtual fBool OnMMouseDown(fFloat X, fFloat Y);
	virtual void OnKeyUp(F2DINPUTKEYCODE KeyCode);
	virtual void OnKeyDown(F2DINPUTKEYCODE KeyCode);
	virtual void OnCharInput(fCharW CharCode);
	virtual void OnQueryControlName(std::wstring& Out);
public:
	fuiContainer(fuiContainer* pParnet);
	~fuiContainer(void);
};
