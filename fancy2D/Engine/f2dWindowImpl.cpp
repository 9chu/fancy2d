#include "f2dWindowImpl.h"

#include "f2dEngineImpl.h"

#include <fcyException.h>
#include <fcyOS/fcyDebug.h>

// #define _IME_DEBUG

using namespace std;

////////////////////////////////////////////////////////////////////////////
// ����
#define F2DWINDOWSTYLENONEBORDER     ( WS_POPUP   | WS_SYSMENU )
#define F2DWINDOWSTYLEFIXEDBORDER    ( WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION )
#define F2DWINDOWSTYLESIZEABLEBORDER ( WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_THICKFRAME )

////////////////////////////////////////////////////////////////////////////////
unordered_map<HWND, f2dWindowImpl*> f2dWindowClass::s_WindowCallBack;

LRESULT CALLBACK f2dWindowClass::WndProc(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ȡָ��
	f2dWindowImpl* pWindow = s_WindowCallBack[Handle];

	// �����ָ�룬����ϵͳ����
	if (pWindow == nullptr)
	{
		return DefWindowProc(Handle,Msg,wParam,lParam);
	}

	// ��ȡ������ָ��
	f2dWindowEventListener* pListener = pWindow->GetListener();

	// ����������Ϣ
	switch(Msg)
	{
	case WM_USER:
		// ���߳�ί��
		if(lParam)
		{
			((f2dMainThreadDelegate*)lParam)->Excute();
			((f2dMainThreadDelegate*)lParam)->Release();
		}
		break;

	case WM_CREATE:
		// ��ʼ��IME������
		pWindow->HandleIMELanguageChanged();
		pWindow->InitIMEContext();
		break;
	case WM_DESTROY:
		// ����IME������
		pWindow->UninitIMEContext();
		break;

		// ��ͨ�ص�
	case WM_CLOSE:
		if(pListener) pListener->OnClose();
		return 0;
	case WM_PAINT:
		if(pListener) pListener->OnPaint();
		break;
	case WM_SIZE:
		{
			fuInt cx = LOWORD(lParam);
			fuInt cy = HIWORD(lParam); 
			if(pListener) pListener->OnSize(cx,cy);
		}
		break;
	case WM_KEYDOWN:
		if(pListener) pListener->OnKeyDown(wParam,lParam);
		break;
	case WM_KEYUP:
		if(pListener) pListener->OnKeyUp(wParam,lParam);
		break;
	case WM_CHAR:
		if(pListener) pListener->OnCharInput((wchar_t)wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		if(pListener) pListener->OnMouseMove(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_MOUSEWHEEL:
		if(pListener) pListener->OnMouseWheel(LOWORD(lParam),HIWORD(lParam),((short)HIWORD(wParam))/(float)WHEEL_DELTA,LOWORD(wParam));
		break;
	case WM_LBUTTONDOWN:
		if(pListener) pListener->OnMouseLBDown(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_LBUTTONUP:
		if(pListener) pListener->OnMouseLBUp(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_LBUTTONDBLCLK:
		if(pListener) pListener->OnMouseLBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_RBUTTONDOWN:
		if(pListener) pListener->OnMouseRBDown(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_RBUTTONUP:
		if(pListener) pListener->OnMouseRBUp(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_RBUTTONDBLCLK:
		if(pListener) pListener->OnMouseRBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_MBUTTONDOWN:
		if(pListener) pListener->OnMouseMBDown(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_MBUTTONUP:
		if(pListener) pListener->OnMouseMBUp(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_MBUTTONDBLCLK:
		if(pListener) pListener->OnMouseMBDouble(LOWORD(lParam),HIWORD(lParam),wParam);
		break;
	case WM_SETFOCUS:
		pWindow->HandleIMELanguageChanged();

		if(pListener) pListener->OnGetFocus();
		break;
	case WM_KILLFOCUS:
		if(pListener) pListener->OnLostFocus();
		break;

		// IME��Ϣ����
	case WM_INPUTLANGCHANGE:
		pWindow->HandleIMELanguageChanged();
		break;
	case WM_IME_STARTCOMPOSITION:
#ifdef _IME_DEBUG
		fcyDebug::Trace(L"[ @ f2dWindowClass::WndProc ] IME start composition.\n");
#endif

		if(pListener) pListener->OnIMEStartComposition();
		break;
	case WM_IME_ENDCOMPOSITION:
#ifdef _IME_DEBUG
		fcyDebug::Trace(L"[ @ f2dWindowClass::WndProc ] IME end composition.\n");
#endif

		if(pListener) pListener->OnIMEEndComposition();
		break;
	case WM_IME_COMPOSITION:
		pWindow->HandleIMEComposition();

		if(pListener) pListener->OnIMEComposition(pWindow->GetIMECompString(), (wchar_t)wParam);
		break;
	case WM_IME_SETCONTEXT:
		// �������뷨���ı���
		if(pWindow->IsHideIME())
		{
			lParam = 0;
			wParam = 0;
		}
		break;
	case WM_IME_NOTIFY:  // ���뷨�¼�
		switch (wParam)
		{
		case IMN_OPENCANDIDATE: // ��ѡ�ʱ�
#ifdef _IME_DEBUG
			fcyDebug::Trace(L"[ @ f2dWindowClass::WndProc ] IME open candidate.\n");
#endif

			pWindow->HandleIMECandidate();

			if(pListener)
			{
				f2dIMECandidateListImpl tList(pWindow);
				pListener->OnIMEOpenCandidate(&tList);
			}
			break;
		case IMN_CLOSECANDIDATE: // �ر�ѡ�ֱ�
#ifdef _IME_DEBUG
			fcyDebug::Trace(L"[ @ f2dWindowClass::WndProc ] IME close candidate.\n");
#endif

			pWindow->HandleIMECandidate();

			if(pListener)
			{
				f2dIMECandidateListImpl tList(pWindow);
				pListener->OnIMECloseCandidate(&tList);
			}
			break;

		case IMN_CHANGECANDIDATE:// ѡ�ֱ�ҳ
#ifdef _IME_DEBUG
			fcyDebug::Trace(L"[ @ f2dWindowClass::WndProc ] IME candidate list changed.\n");
#endif
			pWindow->HandleIMECandidate();

			if(pListener)
			{
				f2dIMECandidateListImpl tList(pWindow);
				pListener->OnIMEChangeCandidate(&tList);
			}
			break;
		}
		break;
	default:
		break;
	}

	// ������Ϣ����ֵ
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
// ��ѡ��
f2dIMECandidateListImpl::f2dIMECandidateListImpl(f2dWindowImpl* pWindow)
{
	m_IMETotalCandidate = pWindow->m_IMETotalCandidate;
	m_IMESelectedCandidate = pWindow->m_IMESelectedCandidate;
	m_IMEPageStartCandidate = pWindow->m_IMEPageStartCandidate;
	m_IMEPageCandidateCount = pWindow->m_IMEPageCandidateCount;
	m_IMECandidateList = pWindow->m_IMECandidateList;
}

f2dIMECandidateListImpl::f2dIMECandidateListImpl(const f2dIMECandidateListImpl& Right)
{
	m_IMETotalCandidate = Right.m_IMETotalCandidate;
	m_IMESelectedCandidate = Right.m_IMESelectedCandidate;
	m_IMEPageStartCandidate = Right.m_IMEPageStartCandidate;
	m_IMEPageCandidateCount = Right.m_IMEPageCandidateCount;
	m_IMECandidateList = Right.m_IMECandidateList;
}

f2dIMECandidateListImpl::~f2dIMECandidateListImpl()
{}

fuInt f2dIMECandidateListImpl::GetCount()
{
	return m_IMETotalCandidate;
}

fuInt f2dIMECandidateListImpl::GetCurIndex()
{
	return m_IMESelectedCandidate;
}

fuInt f2dIMECandidateListImpl::GetPageSize()
{
	return m_IMEPageCandidateCount;
}

fuInt f2dIMECandidateListImpl::GetPageStart()
{
	return m_IMEPageStartCandidate;
}

fcStrW f2dIMECandidateListImpl::GetCandidateStr(fuInt Index)
{
	if(Index > GetCount())
		return NULL;
	else
		return m_IMECandidateList[Index].c_str();
}

////////////////////////////////////////////////////////////////////////////////
// ������
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
void f2dWindowImpl::DefaultListener::OnIMEStartComposition()
{
	m_pEngine->SendMsg(F2DMSG_IME_ONSTARTCOMPOSITION);
}
void f2dWindowImpl::DefaultListener::OnIMEEndComposition()
{
	m_pEngine->SendMsg(F2DMSG_IME_ONENDCOMPOSITION);
}
void f2dWindowImpl::DefaultListener::OnIMEComposition(fcStrW String, fCharW CharCode)
{
	// ��װ�ַ�����������Ϣ
	f2dMsgMemHelper<std::wstring>* tObjMem = new f2dMsgMemHelper<std::wstring>(String);
	m_pEngine->SendMsg(
		F2DMSG_IME_ONCOMPOSITION,
		(fuLong)tObjMem->GetObj().c_str(), 
		CharCode,
		0,
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
}
void f2dWindowImpl::DefaultListener::OnIMEActivated(fcStrW Desc)
{
	// ��װ�ַ�����������Ϣ
	f2dMsgMemHelper<std::wstring>* tObjMem = new f2dMsgMemHelper<std::wstring>(Desc);
	m_pEngine->SendMsg(
		F2DMSG_IME_ONACTIVATE,
		(fuLong)tObjMem->GetObj().c_str(), 
		0,
		0,
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
}
void f2dWindowImpl::DefaultListener::OnIMEClosed()
{
	m_pEngine->SendMsg(F2DMSG_IME_ONCLOSE);
}
void f2dWindowImpl::DefaultListener::OnIMEChangeCandidate(f2dIMECandidateList* pList)
{
	// ��װ��������Ϣ
	f2dMsgMemHelper<f2dIMECandidateListImpl>* tObjMem = new f2dMsgMemHelper<f2dIMECandidateListImpl>(*(f2dIMECandidateListImpl*)pList);
	m_pEngine->SendMsg(
		F2DMSG_IME_ONCHANGECANDIDATE,
		(fuLong)&tObjMem->GetObj(), 
		0,
		0,
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
}
void f2dWindowImpl::DefaultListener::OnIMEOpenCandidate(f2dIMECandidateList* pList)
{
	// ��װ��������Ϣ
	f2dMsgMemHelper<f2dIMECandidateListImpl>* tObjMem = new f2dMsgMemHelper<f2dIMECandidateListImpl>(*(f2dIMECandidateListImpl*)pList);
	m_pEngine->SendMsg(
		F2DMSG_IME_ONOPENCANDIDATE,
		(fuLong)&tObjMem->GetObj(), 
		0,
		0,
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
}
void f2dWindowImpl::DefaultListener::OnIMECloseCandidate(f2dIMECandidateList* pList)
{
	// ��װ��������Ϣ
	f2dMsgMemHelper<f2dIMECandidateListImpl>* tObjMem = new f2dMsgMemHelper<f2dIMECandidateListImpl>(*(f2dIMECandidateListImpl*)pList);
	m_pEngine->SendMsg(
		F2DMSG_IME_ONCLOSECANDIDATE,
		(fuLong)&tObjMem->GetObj(), 
		0,
		0,
		0,
		tObjMem
		);
	FCYSAFEKILL(tObjMem);
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
	: m_DefaultListener(pEngine, this), m_pListener(&m_DefaultListener), m_hWnd(NULL), m_bShow(false), m_CaptionText(Title),
	m_bHideIME(true), m_IMETotalCandidate(0), m_IMESelectedCandidate(0), m_IMEPageStartCandidate(0), m_IMEPageCandidateCount(0)
{
	// ���崰����ʽ
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

	// ���㴰�ڴ�С
	RECT tWinRect = { (int)Pos.a.x , (int)Pos.a.y , (int)Pos.b.x , (int)Pos.b.y};
	AdjustWindowRect(&tWinRect, tWinStyle, FALSE);
	fuInt tRealWidth = tWinRect.right  - tWinRect.left ;
	fuInt tRealHeight = tWinRect.bottom  - tWinRect.top;

	// ��������
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

	// ע�ᴰ��
	f2dWindowClass::s_WindowCallBack[m_hWnd] = this;

	// ��ʾ����
	if(m_bShow)
	{
		SetVisiable(true);
	}
}

f2dWindowImpl::~f2dWindowImpl()
{
	// ���ٴ���
	DestroyWindow(m_hWnd);

	// ȡ��ע��
	unordered_map<HWND, f2dWindowImpl*>::iterator i = f2dWindowClass::s_WindowCallBack.find(m_hWnd);
	if(i != f2dWindowClass::s_WindowCallBack.end())
		f2dWindowClass::s_WindowCallBack.erase(i);
}

void f2dWindowImpl::InitIMEContext()
{
	m_hIMC = ImmCreateContext();
	m_hIMC = ImmAssociateContext(m_hWnd, m_hIMC);
}

void f2dWindowImpl::UninitIMEContext()
{
	m_hIMC = ImmAssociateContext(m_hWnd, m_hIMC);
	ImmDestroyContext(m_hIMC);
}

void f2dWindowImpl::HandleIMELanguageChanged()
{
	HKL hKL = ::GetKeyboardLayout(0);
	int iSize = ::ImmGetDescription(hKL, NULL, 0);
	if( iSize == 0 )
	{
		// ���뷨�ر�
		m_CurIMEDesc.clear();

		if(m_pListener)
			m_pListener->OnIMEClosed();

#ifdef _IME_DEBUG
		fcyDebug::Trace(L"[ @ f2dWindowImpl::HandleIMELanguageChanged ] IME closed.\n");
#endif
	}
	else
	{
		// ���뷨�л�
		m_CurIMEDesc.clear();
		m_CurIMEDesc.resize(iSize);
		ImmGetDescription(hKL, &m_CurIMEDesc[0], iSize);
		
		if(m_pListener)
			m_pListener->OnIMEActivated(GetIMEDesc());

#ifdef _IME_DEBUG
		fcyDebug::Trace(L"[ @ f2dWindowImpl::HandleIMELanguageChanged ] IME changed as : %s.\n", m_CurIMEDesc.c_str());
#endif
	}
}

void f2dWindowImpl::HandleIMEComposition()
{
	HIMC hIMC = ImmGetContext(m_hWnd);
	LONG lSize = ImmGetCompositionString(hIMC, GCS_COMPSTR, 0, 0);

	if( lSize == 0 )
		m_CurIMEComposition.clear();
	else
	{
		m_CurIMEComposition.clear();
		m_CurIMEComposition.resize(lSize);
		ImmGetCompositionString(hIMC, GCS_COMPSTR, &m_CurIMEComposition[0], lSize);
		
#ifdef _IME_DEBUG
		fcyDebug::Trace(L"[ @ f2dWindowImpl::HandleIMEComposition ] Current composition : %s.\n", m_CurIMEComposition.c_str());
#endif

		ImmReleaseContext(m_hWnd, hIMC) ;
	}
}

void f2dWindowImpl::HandleIMECandidate()
{
	// ���������
	HIMC hIMC = ImmGetContext(m_hWnd);

	// ��ú�ѡ���б��С
	LONG dwSize = ImmGetCandidateList(hIMC, 0, NULL, 0);

	if(dwSize == 0)
	{
		m_IMEPageCandidateCount = m_IMEPageStartCandidate = m_IMETotalCandidate = m_IMESelectedCandidate = 0;
		m_IMECandidateList.clear();

		return;
	}

	// ����ȫ�ֿռ�����ź�ѡ��
	LPCANDIDATELIST pList = (LPCANDIDATELIST)GlobalAlloc(GPTR, dwSize);
	
	// ��ú�ѡ���б�
	if(pList)
		ImmGetCandidateList(hIMC, 0, pList, dwSize);
	else
		return;  // �ڴ����ʧ��

	m_IMEPageCandidateCount = pList->dwPageSize;
	m_IMEPageStartCandidate = pList->dwPageStart;
	m_IMETotalCandidate = pList->dwCount;
	m_IMESelectedCandidate = pList->dwSelection;

	// ��ú�ѡ���б�
	m_IMECandidateList.clear();
	m_IMECandidateList.reserve(pList->dwCount);
	for (fuInt i = 0; i<pList->dwCount; ++i)
	{
		fcStrW pStr = (fcStrW)((size_t)pList + (size_t)pList->dwOffset[i]);

		m_IMECandidateList.push_back(pStr);
	}

#ifdef _IME_DEBUG
	wstring tDebugStr;
	for (fuInt i = 0; i<pList->dwCount; ++i)
	{
		tDebugStr += L"\t" + m_IMECandidateList[i] + L"\n";
	}

	fcyDebug::Trace(L"[ @ f2dWindowImpl::HandleIMECandidate ] Candidate info.\n\tCandidate: %d/%d PageStart: %d PageSize: %d\nList :\n%s\n",
		m_IMESelectedCandidate, m_IMETotalCandidate, m_IMEPageStartCandidate, m_IMEPageCandidateCount,
		tDebugStr.c_str());
#endif

	// ��ƨ��
	GlobalFree(pList);
	ImmReleaseContext(m_hWnd, hIMC);
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
	// �����Ļ��С
	fuInt tScreenHalfW = GetSystemMetrics(SM_CXSCREEN) / 2;
	fuInt tScreenHalfH = GetSystemMetrics(SM_CYSCREEN) / 2;

	// ���ԭʼ��С
	fcyRect tRect = GetRect();
	fuInt tHalfW = static_cast<int>(tRect.GetWidth() / 2);
	fuInt tHalfH = static_cast<int>(tRect.GetHeight() / 2);
	float tLeft = static_cast<float>(tScreenHalfW - tHalfW);
	float tTop = static_cast<float>(tScreenHalfH - tHalfH);

	// ���¼�������
	tRect = fcyRect(tLeft, tTop, tLeft + tRect.GetWidth(), tTop + tRect.GetHeight());

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

void f2dWindowImpl::HideMouse(fBool bHide)
{
	ShowCursor((BOOL)!bHide);
}
