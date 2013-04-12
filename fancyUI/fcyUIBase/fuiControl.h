#pragma once

#include "fuiContext.h"
#include "fuiRenderer.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI鼠标原事件
////////////////////////////////////////////////////////////////////////////////
enum FUIMOUSEEVENT
{
	FUIMOUSEEVENT_MOVE,  ///< @brief 鼠标移动
	FUIMOUSEEVENT_LDOWN, ///< @brief 左键按下
	FUIMOUSEEVENT_LUP,   ///< @brief 左键放开
	FUIMOUSEEVENT_RDOWN, ///< @brief 右键按下
	FUIMOUSEEVENT_RUP,   ///< @brief 右键放开
	FUIMOUSEEVENT_MDOWN, ///< @brief 中键按下
	FUIMOUSEEVENT_MUP    ///< @brief 中键放开
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI键盘原事件
////////////////////////////////////////////////////////////////////////////////
enum FUIKEYBOARDEVENT
{
	FUIKEYBOARDEVENT_KEYDOWN,  ///< @brief 按键按下
	FUIKEYBOARDEVENT_KEYUP,    ///< @brief 按键放开
	FUIKEYBOARDEVENT_CHARINPUT ///< @brief 输入的字符
};

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI辅助
////////////////////////////////////////////////////////////////////////////////
class fuiHelper
{
private:
	static const F2DINPUTKEYCODE VKCodeToF2DKeyCodeTable[256];
public: // 辅助操作
	static F2DINPUTKEYCODE VKCodeToF2DKeyCode(fByte VKCode);
	static fBool AttributeToBool(fcyXmlNode* pNode, fcStrW Attribute, fBool& pOut);
	static fBool AttributeToFloat(fcyXmlNode* pNode, fcStrW Attribute, fFloat& pOut);
	static fBool AttributeToRect(fcyXmlNode* pNode, fcStrW Attribute, fcyRect& pOut);
	static fBool AttributeToVec2(fcyXmlNode* pNode, fcStrW Attribute, fcyVec2& pOut);
	static fBool AttributeToVec3(fcyXmlNode* pNode, fcStrW Attribute, fcyVec3& pOut);
	static fBool AttributeToSprite(fuiContext* pContext, fcyXmlNode* pNode, fcStrW Attribute, f2dSprite* &pOut);
};

class fuiContainer;

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI控件
////////////////////////////////////////////////////////////////////////////////
class fuiControl
{
	friend class fuiContainer;
private:
	static const fCharW ControlName[];
private: // 私有属性
	std::wstring m_StyleName;///< @brief 风格名称
	fuiContainer* m_pParent; ///< @brief 父容器（可NULL）
	fcyRect m_UIRect;        ///< @brief UI位置
	fcyVec2 m_UIMinSize;     ///< @brief UI最小大小
	fBool m_bVisiable;       ///< @brief 是否可视(render)
	fBool m_bUpdate;         ///< @brief 是否更新(update)
	fBool m_bClip;           ///< @brief 是否剪切
protected: // 私有操作
	/// @brief 设置控件的最小大小
	/// @note  若当前大小小于最小大小则会调用SetRect设置到最小大小
	void setMinSize(const fcyVec2& Size);
public: // 控件操作
	/// @brief 获得顶层对象
	fuiControl* GetTopParent()const;
	/// @brief 获得上下文
	fuiContext* GetContext();
	/// @brief 获得最小大小
	const fcyVec2& GetMinSize()const;
	/// @brief 获得父类
	fuiContainer* GetParent()const;
	/// @brief 设置父类
	void SetParent(fuiContainer* pParent);
	/// @brief 获得层次，0为最上层
	fuInt GetControlLayer();
	/// @brief 设置层次
	void SetControlLayer(fuInt Index);
	/// @brief 是否可视
	fBool IsVisiable()const;
	/// @brief 设置可视
	void SetVisiable(fBool bVisiable);
	/// @brief 是否更新
	fBool IsUpdatable()const;
	/// @brief 设置更新
	void SetUpdatable(fBool bUpdatable);
	/// @brief 是否裁剪
	fBool IsClip()const;
	/// @brief 设置裁剪
	void SetClip(fBool bClip);
	/// @brief 获得矩形
	const fcyRect& GetRect()const;
	/// @brief 设置矩形
	void SetRect(const fcyRect& Range);
	/// @brief  返回样式
	/// @return 无样式返回NULL
	fcStrW GetStyle();
	/// @brief 获得样式
	void SetStyle(fcStrW pStyle);
	/// @brief 获得焦点
	void GetFocus();
	/// @brief 丢失焦点
	void LostFocus();
	/// @brief     渲染
	/// @param[in] pRenderer 渲染器
	void Render(fuiRenderer* pRenderer);
	/// @brief     更新
	/// @param[in] ElapsedTime 流逝时间
	void Update(fDouble ElapsedTime);
	/// @brief     发送鼠标消息
	/// @param[in] Type 鼠标消息类型
	/// @param[in] X    相对父控件位置
	/// @param[in] Y    相对父控件位置
	/// @return    是否接受消息
	fBool SendMouseEvent(FUIMOUSEEVENT Type, fFloat X, fFloat Y);
	/// @brief     发送键盘消息
	/// @param[in] Type    键盘消息类型
	/// @param[in] KeyCode 键盘代码
	/// @return    是否接受消息
	fBool SendKeyboardEvent(FUIKEYBOARDEVENT Type, F2DINPUTKEYCODE KeyCode);
protected: // 可覆写事件
	virtual void OnRender(fuiRenderer* pRenderer);      ///< @brief 渲染事件
	virtual void OnUpdate(fDouble ElapsedTime);         ///< @brief 更新事件
	virtual void OnResized(fcyRect& NewSize);           ///< @brief 大小改变事件
	virtual void OnParentChanged(fuiContainer* pOld);   ///< @brief 父对象改变
	virtual void OnStyleChanged(fcStrW NewStyleName);   ///< @brief 外观改变
	virtual void OnVisiableChanged();                   ///< @brief 可视改变
	virtual fBool OnGetFocus();                         ///< @brief 获得焦点事件
	virtual void OnLostFocus();                         ///< @brief 丢失焦点事件
	virtual void OnMouseLeave();                        ///< @brief 鼠标移出事件
	virtual fBool OnMouseMove(fFloat X, fFloat Y);      ///< @brief 鼠标移动事件
	virtual void OnLMouseUp(fFloat X, fFloat Y);        ///< @brief 鼠标左键放开
	virtual fBool OnLMouseDown(fFloat X, fFloat Y);     ///< @brief 鼠标左键按下
	virtual void OnRMouseUp(fFloat X, fFloat Y);        ///< @brief 鼠标右键放开
	virtual fBool OnRMouseDown(fFloat X, fFloat Y);     ///< @brief 鼠标右键按下
	virtual void OnMMouseUp(fFloat X, fFloat Y);        ///< @brief 鼠标中键放开
	virtual fBool OnMMouseDown(fFloat X, fFloat Y);     ///< @brief 鼠标中间按下
	virtual void OnKeyUp(F2DINPUTKEYCODE KeyCode);      ///< @brief 键盘按键放开
	virtual void OnKeyDown(F2DINPUTKEYCODE KeyCode);    ///< @brief 键盘按键按下
	virtual void OnCharInput(fCharW CharCode);          ///< @brief 输入字符
	virtual void OnQuerySubControlOffset(fcyVec2& pOut);///< @brief 查询子控件基础位移
	virtual void OnQueryControlName(std::wstring& Out); ///< @brief 查询控件类型名
	virtual const fcyRect& OnQueryClipRect();           ///< @brief 返回裁剪矩形
	virtual fuiContext* OnQueryContext();               ///< @brief 获得上下文
public:
	fuiControl(fuiContainer* pParent);
	virtual ~fuiControl(void);
};
