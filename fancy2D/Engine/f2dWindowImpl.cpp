#include "f2dWindowImpl.h"

#include "f2dEngineImpl.h"

#include "fcyException.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// 常量
#define F2DWINDOWSTYLENONEBORDER     ( WS_POPUP   | WS_SYSMENU )
#define F2DWINDOWSTYLEFIXEDBORDER    ( WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION )
#define F2DWINDOWSTYLESIZEABLEBORDER ( WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_THICKFRAME )

////////////////////////////////////////////////////////////////////////////////
unordered_map<HWND, f2dWindowImpl*> f2dWindowClass::s_WindowCallBack;

LRESULT CALLBACK f2dWindowClass::WndProc(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 提取指针
	f2dWindowImpl* pWindow = s_WindowCallBack[Handle];

	// 如果无指针，交给系统处理
	if (pWindow == nullptr)
	{
		return DefWindowProc(Handle,Msg,wParam,lParam);
	}

	// 提取监听器指针
	f2dWindowEventListener* pListener = pWindow->GetListener();

	// 如果无指针，处理关闭消息
	if(!pListener)
	{
		if(Msg == WM_CLOSE)
			return 0;
	}
	else
	{
		// 否则 处理并派送消息
		switch(Msg)
		{
		case WM_CLOSE:
			pListener->OnClose();
			return 0;
		case WM_PAINT:
			pListener->OnPaint();
			break;
		case WM_SIZE:
			{
				fuInt cx = LOWORD(lParam);
				fuInt cy = HIWORD(lParam); 
				pListener->OnSize(cx,cy);
			}
			break;
		case WM_KEYDOWN:
			pListener->OnKeyDown(wParam,lParam);
			break;
		case WM_KEYUP:
			pListener->OnKeyUp(wParam,lParam);
			break;
		case WM_CHAR:
			pListener->OnCharInput((wchar_t)wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			pListener->OnMouseMove(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_MOUSEWHEEL:
			pListener->OnMouseWheel(LOWORD(lParam),HIWORD(lParam),((short)HIWORD(wParam))/(float)WHEEL_DELTA,LOWORD(wParam));
			break;
		case WM_LBUTTONDOWN:
			pListener->OnMouseLBDown(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_LBUTTONUP:
			pListener->OnMouseLBUp(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_LBUTTONDBLCLK:
			pListener->OnMouseLBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_RBUTTONDOWN:
			pListener->OnMouseRBDown(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_RBUTTONUP:
			pListener->OnMouseRBUp(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_RBUTTONDBLCLK:
			pListener->OnMouseRBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_MBUTTONDOWN:
			pListener->OnMouseMBDown(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_MBUTTONUP:
			pListener->OnMouseMBUp(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_MBUTTONDBLCLK:
			pListener->OnMouseMBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
			break;
		case WM_IME_STARTCOMPOSITION:
			pListener->OnIMEStartComposition();
			break;
		case WM_IME_ENDCOMPOSITION:
			pListener->OnIMEEndComposition();
			break;
		case WM_IME_COMPOSITION:
			pListener->OnIMEComposition((wchar_t) wParam, lParam);
			break;
		case WM_SETFOCUS:
			pListener->OnGetFocus();
			break;
		case WM_KILLFOCUS:
			pListener->OnLostFocus();
			break;
		default:
			break;
		}
	}

	// 处理消息返回值
	return DefWindowProc(Handle,Msg,wParam,lParam);
}

f2dWindowClass::f2dWindowClass(f2dEngineImpl* pEngine, fcStrW ClassName)
	: m_pEngine(pEngine), m_ClsName(ClassName)
{
	WNDCLASS tWndClass;
	tWndClass.style = CS_HREDRAW | CS_VREDRAW;
	tWndClass.lpfnWndProc = WndProc;
	tWndClass.cbClsExtra = 0;
	tWndClass.cbWndExtra = 0;
	tWndClass.hInstance = GetModuleHandle(NULL);
	tWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	tWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	tWndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	tWndClass.lpszMenuName = NULL;
	tWndClass.lpszClassName = m_ClsName.c_str();
	
	ATOM tRet = RegisterClass(&tWndClass);
	if(!tRet)
		throw fcyWin32Exception("f2dWindowClass::f2dWindowClass", "RegisterClass Failed.");
}

f2dWindowClass::~f2dWindowClass()
{
	UnregisterClass(m_ClsName.c_str(), GetModuleHandle(NULL));
}

fcStrW f2dWindowClass::GetName()const
{
	return m_ClsName.c_str();
}

f2dWindowImpl* f2dWindowClass::CreateRenderWindow(const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border)
{
	return new f2dWindowImpl(m_pEngine, this, Pos, Title, Visiable, Border);
}

////////////////////////////////////////////////////////////////////////////////

f2dWindowDC::f2dWindowDC()
{
	m_pBits = NULL;
    m_hBmp  = NULL;
	m_hDC	= NULL;
	m_Width = 0;
	m_Height= 0;
}

f2dWindowDC::~f2dWindowDC()
{
	Delete();
}

void f2dWindowDC::Delete()
{
	if (m_hBmp)
		DeleteObject(m_hBmp);
	if (m_hDC)
		DeleteDC(m_hDC);
}

void f2dWindowDC::Create(int nWidth, int nHeight)
{
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	bih.biSize	      =	sizeof(BITMAPINFOHEADER);
	bih.biWidth       =	nWidth;
	bih.biHeight      = -nHeight;
	bih.biPlanes      = 1;
	bih.biBitCount    = 32;
	bih.biCompression = BI_RGB;
	m_hBmp = NULL;
	m_hDC  = CreateCompatibleDC(NULL);
	m_hBmp = ::CreateDIBSection(
		GetSafeHdc(), (BITMAPINFO*)&bih,
		DIB_RGB_COLORS, (void**)(&m_pBits), NULL, 0);
	SelectObject(m_hDC, m_hBmp);

	m_Width = nWidth;
	m_Height = nHeight;
}

////////////////////////////////////////////////////////////////////////////////
// 监听器
void f2dWindowImpl::DefaultListener::OnClose() 
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONCLOSE);
}
void f2dWindowImpl::DefaultListener::OnPaint()
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONPAINT);
}
void f2dWindowImpl::DefaultListener::OnSize(fuInt ClientWidth, fuInt ClientHeight)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONRESIZE, ClientWidth, ClientHeight);
}
void f2dWindowImpl::DefaultListener::OnKeyDown(fuInt KeyCode, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONKEYDOWN, KeyCode);
}
void f2dWindowImpl::DefaultListener::OnKeyUp(fuInt KeyCode, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONKEYUP, KeyCode);
}
void f2dWindowImpl::DefaultListener::OnCharInput(fCharW CharCode, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONCHARINPUT, CharCode);
}
void f2dWindowImpl::DefaultListener::OnMouseMove(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSEMOVE, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseWheel(fShort X, fShort Y, fFloat Wheel, fuInt Flag)
{
	fDouble tValue = Wheel;
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSEWHEEL, X, Y, *(fuLong*)&tValue);
}
void f2dWindowImpl::DefaultListener::OnMouseLBDown(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSELDOWN, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseLBUp(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSELUP, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseLBDouble(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSELDOUBLE, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseMBDown(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSEMDOWN, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseMBUp(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSEMUP, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseMBDouble(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSEMDOUBLE, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseRBDown(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSERDOWN, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseRBUp(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSERUP, X, Y);
}
void f2dWindowImpl::DefaultListener::OnMouseRBDouble(fShort X, fShort Y, fuInt Flag)
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONMOUSERDOUBLE, X, Y);
}
void f2dWindowImpl::DefaultListener::OnGetFocus()
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONGETFOCUS);
}
void f2dWindowImpl::DefaultListener::OnLostFocus()
{
	m_pEngine->SendMsg(F2DMSG_WINDOW_ONLOSTFOCUS);
}

////////////////////////////////////////////////////////////////////////////////

f2dWindowImpl::f2dWindowImpl(f2dEngineImpl* pEngine, f2dWindowClass* WinCls, const fcyRect& Pos, fcStrW Title, fBool Visiable, F2DWINBORDERTYPE Border)
	: m_DefaultListener(pEngine), m_pListener(&m_DefaultListener), m_hWnd(NULL), m_bShow(false), m_CaptionText(Title)
{
	// 定义窗口样式
	fuInt tWinStyle;
	switch(Border)
	{
	case F2DWINBORDERTYPE_NONE:
		tWinStyle = F2DWINDOWSTYLENONEBORDER;
		break;
	case F2DWINBORDERTYPE_FIXED:
		tWinStyle = F2DWINDOWSTYLEFIXEDBORDER;
		break;
	case F2DWINBORDERTYPE_SIZEABLE:
		tWinStyle = F2DWINDOWSTYLESIZEABLEBORDER;
		break;
	default:
		throw fcyException("f2dWindowImpl::f2dWindowImpl", "Invalid F2DWINBORDERTYPE.");
	}

	// 计算窗口大小
	RECT tWinRect = { (int)Pos.a.x , (int)Pos.a.y , (int)Pos.b.x , (int)Pos.b.y};
	AdjustWindowRect(&tWinRect, tWinStyle, FALSE);
	fuInt tRealWidth = tWinRect.right  - tWinRect.left ;
	fuInt tRealHeight = tWinRect.bottom  - tWinRect.top;

	// 创建窗口
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		WinCls->GetName(),
		m_CaptionText.c_str(),
		tWinStyle,
		tWinRect.left,
		tWinRect.top,
		tRealWidth,
		tRealHeight,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
		);

	if(!m_hWnd)
		throw fcyWin32Exception("f2dWindowImpl::f2dWindowImpl", "CreateWindowEx Failed.");

	// 注册窗口
	f2dWindowClass::s_WindowCallBack[m_hWnd] = this;

	// 显示窗口
	if(m_bShow)
	{
		SetVisiable(true);
	}
}

f2dWindowImpl::~f2dWindowImpl()
{
	// 销毁窗口
	DestroyWindow(m_hWnd);

	// 取消注册
	unordered_map<HWND, f2dWindowImpl*>::iterator i = f2dWindowClass::s_WindowCallBack.find(m_hWnd);
	if(i != f2dWindowClass::s_WindowCallBack.end())
		f2dWindowClass::s_WindowCallBack.erase(i);
}

f2dWindowEventListener* f2dWindowImpl::GetListener()
{
	return m_pListener;
}

fResult f2dWindowImpl::SetListener(f2dWindowEventListener* pListener)
{
	m_pListener = pListener;
	return FCYERR_OK;
}

fInt f2dWindowImpl::GetHandle()
{
	return (fInt)m_hWnd;
}

F2DWINBORDERTYPE f2dWindowImpl::GetBorderType()
{
	fuInt tStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	switch(tStyle)
	{
	case F2DWINDOWSTYLENONEBORDER:
		return F2DWINBORDERTYPE_NONE;
	case F2DWINDOWSTYLEFIXEDBORDER:
		return F2DWINBORDERTYPE_FIXED;
	case F2DWINDOWSTYLESIZEABLEBORDER:
		return F2DWINBORDERTYPE_SIZEABLE;
	default:
		return F2DWINBORDERTYPE_NONE;
	}
}

fResult f2dWindowImpl::SetBorderType(F2DWINBORDERTYPE Type)
{
	switch(Type)
	{
	case F2DWINBORDERTYPE_NONE:
		SetWindowLong(m_hWnd, GWL_STYLE, F2DWINDOWSTYLENONEBORDER);
		break;
	case F2DWINBORDERTYPE_FIXED:
		SetWindowLong(m_hWnd, GWL_STYLE, F2DWINDOWSTYLEFIXEDBORDER);
		break;
	case F2DWINBORDERTYPE_SIZEABLE:
		SetWindowLong(m_hWnd, GWL_STYLE, F2DWINDOWSTYLESIZEABLEBORDER);
		break;
	default:
		return FCYERR_ILLEGAL;
	}

	SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, m_bShow ? SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE : SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);
	return FCYERR_OK;
}

fcStrW f2dWindowImpl::GetCaption()
{
	wchar_t tTemp[1024];
	GetWindowText(m_hWnd, tTemp, 1024);
	m_CaptionText = tTemp;
	return m_CaptionText.c_str();
}

fResult f2dWindowImpl::SetCaption(fcStrW Caption)
{
	m_CaptionText = Caption;
	return SetWindowText(m_hWnd, Caption)==TRUE?FCYERR_OK : FCYERR_INTERNALERR;
}

fBool f2dWindowImpl::GetVisiable()
{
	return m_bShow;
}

fResult f2dWindowImpl::SetVisiable(fBool Visiable)
{
	m_bShow = Visiable;
	return ShowWindow(m_hWnd, m_bShow?SW_SHOW:SW_HIDE)==TRUE?FCYERR_OK : FCYERR_INTERNALERR;
}

fcyRect f2dWindowImpl::GetRect()
{
	RECT tRect;
	GetWindowRect(m_hWnd, &tRect);
	fcyRect tRet((float)tRect.left, (float)tRect.top, (float)tRect.right, (float)tRect.bottom);
	return tRet;
}

fResult f2dWindowImpl::SetRect(const fcyRect& Range)
{
	return SetWindowPos(m_hWnd, 0, (int)Range.a.x, (int)Range.a.y, (int)Range.GetWidth(), (int)Range.GetHeight(), SWP_NOZORDER)==TRUE?FCYERR_OK : FCYERR_INTERNALERR;
}

fcyRect f2dWindowImpl::GetClientRect()
{
	RECT tRect;
	::GetClientRect(m_hWnd, &tRect);
	fcyRect tRet((float)tRect.left, (float)tRect.top, (float)tRect.right, (float)tRect.bottom);
	return tRet;
}

fResult f2dWindowImpl::SetClientRect(const fcyRect& Range)
{
	RECT tWinRect = { (int)Range.a.x , (int)Range.a.y , (int)Range.b.x , (int)Range.b.y};
	AdjustWindowRect(&tWinRect, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
	return SetWindowPos(m_hWnd, 0, tWinRect.left, tWinRect.top, tWinRect.right - tWinRect.left, tWinRect.bottom - tWinRect.top, SWP_NOZORDER)==TRUE?FCYERR_OK : FCYERR_INTERNALERR;
}

void f2dWindowImpl::MoveToCenter()
{
	// 获得屏幕大小
	fuInt tScreenHalfW = GetSystemMetrics(SM_CXSCREEN) / 2;
	fuInt tScreenHalfH = GetSystemMetrics(SM_CYSCREEN) / 2;

	// 获得原始大小
	fcyRect tRect = GetRect();
	fuInt tHalfW = (int)tRect.GetWidth() / 2;
	fuInt tHalfH = (int)tRect.GetHeight() / 2;

	// 重新计算坐标
	tRect = fcyRect((float)tScreenHalfW - tHalfW, (float)tScreenHalfH - tHalfH, 
		(float)tScreenHalfW + tHalfW, (float)tScreenHalfH + tHalfH);

	SetRect(tRect);
}

fBool f2dWindowImpl::IsTopMost()
{
	if(WS_EX_TOPMOST & GetWindowLong(m_hWnd, GWL_EXSTYLE))
		return true;
	else
		return false;
}

fResult f2dWindowImpl::SetTopMost(fBool TopMost)
{
	if(TopMost)
		if(SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == FALSE)
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
	else
		if(SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == FALSE)
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
}
