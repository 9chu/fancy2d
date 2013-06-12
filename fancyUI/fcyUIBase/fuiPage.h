////////////////////////////////////////////////////////////////////////////////
/// @file  fuiPage.h
/// @brief fancyUI 根控件
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include <fcyParser/fcyXml.h>

#include "fuiControl.h"
#include "fuiStyle.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyUI 根控件
////////////////////////////////////////////////////////////////////////////////
class fuiPage :
	public fuiControl
{
	friend class fuiControl;
public:
	/// @brief 鼠标按键
	enum MOUSEBUTTON
	{
		MOUSEBUTTON_L,  ///< @brief 鼠标左键
		MOUSEBUTTON_M,  ///< @brief 鼠标中键
		MOUSEBUTTON_R   ///< @brief 鼠标右键
	};
public:
	static F2DINPUTKEYCODE VKKeyToF2DKey(fuInt VKCode);
protected:
	/// @brief 控件表
	std::unordered_map<std::wstring, fuiControl*> m_ControlMap;

	/// @brief 默认样式
	fcyRefPointer<fuiStyle> m_pDefaultStyle;

	/// @brief 渲染器
	fuiGraphics m_pGraphics;

	/// @brief DEBUG开关
	fBool m_bDebug;

	/// @brief 鼠标位置的控件
	fuiControl* m_pControlAtMousePos;

	/// @brief 鼠标位置的控件的相对鼠标位置
	fcyVec2 m_ControlMousePos;

	/// @brief 上一个接受鼠标移动信息的控件左上角坐标
	fcyVec2 m_ControlOrigin;

	/// @brief 上一个接受鼠标移动信息的控件
	fuiControl* m_pLastMouseMoveControl;

	/// @brief 鼠标锁定对象
	fuiControl* m_pLockMouseControl;

	/// @brief 鼠标解锁消息
	MOUSEBUTTON m_MouseUnlockEvent;

	/// @brief 焦点 （上一个鼠标锁定对象）
	fuiControl* m_pFocus;
private:
	/// @brief  加载布局节点
	void loadLayoutNode(fcyXmlNode* pNode, fuiControl* pParent);
	/// @brief  DEBUG绘图
	void debugDraw(fuiGraphics* pGraph, fuiControl* pControl);
	/// @brief     获得鼠标处的控件
	/// @param[in] pControl 要测试的控件
	/// @param[in] Pos      鼠标相对父控件的位置
	fuiControl* getControlAtPos(fuiControl* pControl, const fcyVec2& Pos, fcyVec2& PosOut);
	/// @brief 投递子对象丢失焦点消息
	void sendSubControlLostFocusMsg(fuiControl* pControl)
	{
		fuiObjectEventArgs tArgs;
		tArgs.SetObj(pControl);

		fuiControl* p = pControl->GetParent();

		while(p && p != p->GetRoot())
		{
			p->ExecEvent(L"OnSubcontrolLostFocus", &tArgs);
			p = p->GetParent();
		}
	}
	/// @brief 投递子对象获得焦点消息
	void sendSubControlGetFocusMsg(fuiControl* pControl)
	{
		fuiObjectEventArgs tArgs;
		tArgs.SetObj(pControl);

		fuiControl* p = pControl->GetParent();

		while(p && p != p->GetRoot())
		{
			p->ExecEvent(L"OnSubcontrolGetFocus");
			p = p->GetParent();
		}
	}
protected: // for fuiControl
	/// @brief  注册一个控件
	void RegisterControl(fuiControl* pControl);
	/// @brief  取消控件注册
	void UnregisterControl(fuiControl* pControl);
public:
	/// @brief  加载布局文件
	void LoadLayoutFromFile(f2dStream* pStream);
	/// @brief  寻找控件
	/// @return 控件弱引用或者NULL
	fuiControl* FindControl(const std::wstring& Name);
	/// @brief  DEBUG模式
	void SetDebugMode(fBool Value) { m_bDebug = Value; }
public: // 接口实现
	fuiStyle* GetControlStyle()const;
	void SetControlStyle(fuiStyle* pStyle);

	void Update(fDouble ElapsedTime);
	void Render(fuiGraphics* pGraph=NULL);
public: // UI消息
	/// @brief     处理鼠标移动消息
	/// @param[in] MousePos 相对于父控件（屏幕）的位置
	void SendMouseMove(const fcyVec2& MousePos);
	/// @brief     处理鼠标按下消息
	/// @param[in] Button   按钮
	/// @param[in] MousePos 可选鼠标位置，重新发送消息到SendMouseMove
	void SendMouseButtonDown(MOUSEBUTTON Button, fcyVec2* MousePos);
	/// @brief     处理鼠标放开消息
	/// @param[in] Button   按钮
	/// @param[in] MousePos 可选鼠标位置，重新发送消息到SendMouseMove
	void SendMouseButtonUp(MOUSEBUTTON Button, fcyVec2* MousePos);
	/// @brief 发送键盘按键按下消息
	void SendKeyDown(F2DINPUTKEYCODE Key);
	/// @brief 发送键盘按键放开消息
	void SendKeyUp(F2DINPUTKEYCODE Key);
	/// @brief 发送滚轮事件
	void SendMouseWheel(fDouble Value);
	/// @brief 发送字符输入事件
	void SendCharInput(fCharW Char);
public:
	fuiPage(const std::wstring& Name, f2dRenderer* pRenderer, f2dGraphics2D* pGraph);
protected:
	~fuiPage();
};
