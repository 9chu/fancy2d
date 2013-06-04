////////////////////////////////////////////////////////////////////////////////
/// @file  f2dWindowImpl.h
/// @brief fancy2D渲染窗口接口实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fcyRefObj.h"
#include "fcyIO\fcyStream.h"

#include "../f2dWindow.h"

#include <string>
#include <unordered_map>

#include <Windows.h>

class f2dEngineImpl;

////////////////////////////////////////////////////////////////////////////////
/// @brief 窗口类
////////////////////////////////////////////////////////////////////////////////
class f2dWindowClass
{
	friend class f2dWindowImpl;
private:
	f2dEngineImpl* m_pEngine;
	std::wstring m_ClsName;
private:
	static std::unordered_map<HWND, f2dWindowImpl*> s_WindowCallBack;
	static LRESULT CALLBACK WndProc(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam);
public:
	fcStrW GetName()const; // 获得窗口类名称
	f2dWindowImpl* CreateRenderWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border); // 创建窗口
public:
	f2dWindowClass(f2dEngineImpl* pEngine, fcStrW ClassName);
	~f2dWindowClass();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 窗口DC
////////////////////////////////////////////////////////////////////////////////
class f2dWindowDC
{
protected:
	HBITMAP	m_hBmp;
	HDC		m_hDC;
	char*   m_pBits;

	fInt m_Width;
	fInt m_Height;
public:
	void Delete();
	void Create(int nWidth, int nHeight);
	HDC GetSafeHdc(void){
		return m_hDC;
	}
	HBITMAP GetBmpHandle(void){
		return m_hBmp;
	}
	DWORD* GetBits(void){
		return (DWORD*)m_pBits;
	}
	fInt GetWidth()
	{
		return m_Width;
	}
	fInt GetHeight()
	{
		return m_Height;
	}
public:
	f2dWindowDC();
    ~f2dWindowDC();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief 渲染窗口
////////////////////////////////////////////////////////////////////////////////
class f2dWindowImpl :
	public fcyRefObjImpl<f2dWindow>
{
	friend class f2dWindowClass;
private:
	class DefaultListener :
		public f2dWindowEventListener
	{
	protected:
		f2dEngineImpl* m_pEngine;
	public:
		void OnIMEStartComposition() {}
		void OnIMEEndComposition() {}
		void OnIMEComposition(fCharW CharCode, fuInt Flag) {}

		void OnClose();
		void OnPaint();
		void OnSize(fuInt ClientWidth, fuInt ClientHeight);
		void OnKeyDown(fuInt KeyCode, fuInt Flag);
		void OnKeyUp(fuInt KeyCode, fuInt Flag);
		void OnCharInput(fCharW CharCode, fuInt Flag);
		void OnMouseMove(fShort X, fShort Y, fuInt Flag);
		void OnMouseWheel(fShort X, fShort Y, fFloat Wheel, fuInt Flag);
		void OnMouseLBDown(fShort X, fShort Y, fuInt Flag);
		void OnMouseLBUp(fShort X, fShort Y, fuInt Flag);
		void OnMouseLBDouble(fShort X, fShort Y, fuInt Flag);
		void OnMouseMBDown(fShort X, fShort Y, fuInt Flag);
		void OnMouseMBUp(fShort X, fShort Y, fuInt Flag);
		void OnMouseMBDouble(fShort X, fShort Y, fuInt Flag);
		void OnMouseRBDown(fShort X, fShort Y, fuInt Flag);
		void OnMouseRBUp(fShort X, fShort Y, fuInt Flag);
		void OnMouseRBDouble(fShort X, fShort Y, fuInt Flag);
		void OnGetFocus();
		void OnLostFocus();
	public:
		DefaultListener(f2dEngineImpl* pEngine)
			: m_pEngine(pEngine) {}
	};
private:
	// 状态
	HWND m_hWnd;
	bool m_bShow;
	std::wstring m_CaptionText;

	// 监听器
	DefaultListener m_DefaultListener;
	f2dWindowEventListener* m_pListener;
public: // 接口实现
	f2dWindowEventListener* GetListener();
	fResult SetListener(f2dWindowEventListener* pListener);

	fInt GetHandle();

	F2DWINBORDERTYPE GetBorderType();
	fResult SetBorderType(F2DWINBORDERTYPE Type);
	fcStrW GetCaption();
	fResult SetCaption(fcStrW Caption);
	fBool GetVisiable();
	fResult SetVisiable(fBool Visiable);
	fcyRect GetRect();
	fResult SetRect(const fcyRect& Range);
	fcyRect GetClientRect();
	fResult SetClientRect(const fcyRect& Range);
	void MoveToCenter();
	fBool IsTopMost();
	fResult SetTopMost(fBool TopMost);
protected:
	f2dWindowImpl(f2dEngineImpl* pEngine, f2dWindowClass* WinCls, const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border);
	~f2dWindowImpl();
};
