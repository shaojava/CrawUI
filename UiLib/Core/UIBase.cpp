#include "StdAfx.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace UiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
    TCHAR szBuffer[2048] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    ::wvnsprintf(szBuffer, lengthof(szBuffer) - 2, pstrFormat, args);
    _tcscat(szBuffer, _T("\r\n"));
    va_end(args);
    ::OutputDebugString(szBuffer);
#endif
}

//************************************
// 函数名称: DUI__Trace
// 返回类型: void UILIB_API
// 参数信息: LPCSTR pstrFormat
// 参数信息: ...
// 函数说明: 
//************************************
void UILIB_API DUI__Trace( LPCSTR pstrFormat,... )
{
#ifdef _DEBUG
	CHAR szBuffer[2048] = { 0 };
	va_list args;
	va_start(args,pstrFormat);
	vsprintf_s(szBuffer,lengthof(szBuffer),pstrFormat,args);
	strcat(szBuffer, "\r\n");
	va_end(args);
	::OutputDebugStringA(szBuffer);
#endif
}

LPCTSTR DUI__TraceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return _T(#x)
    MSGDEF(WM_SETCURSOR);
    MSGDEF(WM_NCHITTEST);
    MSGDEF(WM_NCPAINT);
    MSGDEF(WM_PAINT);
    MSGDEF(WM_ERASEBKGND);
    MSGDEF(WM_NCMOUSEMOVE);  
    MSGDEF(WM_MOUSEMOVE);
    MSGDEF(WM_MOUSELEAVE);
    MSGDEF(WM_MOUSEHOVER);   
    MSGDEF(WM_NOTIFY);
    MSGDEF(WM_COMMAND);
    MSGDEF(WM_MEASUREITEM);
    MSGDEF(WM_DRAWITEM);   
    MSGDEF(WM_LBUTTONDOWN);
    MSGDEF(WM_LBUTTONUP);
    MSGDEF(WM_LBUTTONDBLCLK);
    MSGDEF(WM_RBUTTONDOWN);
    MSGDEF(WM_RBUTTONUP);
    MSGDEF(WM_RBUTTONDBLCLK);
    MSGDEF(WM_SETFOCUS);
    MSGDEF(WM_KILLFOCUS);  
    MSGDEF(WM_MOVE);
    MSGDEF(WM_SIZE);
    MSGDEF(WM_SIZING);
    MSGDEF(WM_MOVING);
    MSGDEF(WM_GETMINMAXINFO);
    MSGDEF(WM_CAPTURECHANGED);
    MSGDEF(WM_WINDOWPOSCHANGED);
    MSGDEF(WM_WINDOWPOSCHANGING);   
    MSGDEF(WM_NCCALCSIZE);
    MSGDEF(WM_NCCREATE);
    MSGDEF(WM_NCDESTROY);
    MSGDEF(WM_TIMER);
    MSGDEF(WM_KEYDOWN);
    MSGDEF(WM_KEYUP);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SYSKEYDOWN);
    MSGDEF(WM_SYSKEYUP);
    MSGDEF(WM_SYSCOMMAND);
    MSGDEF(WM_SYSCHAR);
    MSGDEF(WM_VSCROLL);
    MSGDEF(WM_HSCROLL);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SHOWWINDOW);
    MSGDEF(WM_PARENTNOTIFY);
    MSGDEF(WM_CREATE);
    MSGDEF(WM_NCACTIVATE);
    MSGDEF(WM_ACTIVATE);
    MSGDEF(WM_ACTIVATEAPP);   
    MSGDEF(WM_CLOSE);
    MSGDEF(WM_DESTROY);
    MSGDEF(WM_GETICON);   
    MSGDEF(WM_GETTEXT);
    MSGDEF(WM_GETTEXTLENGTH);   
    static TCHAR szMsg[10];
    ::wsprintf(szMsg, _T("0x%04X"), uMsg);
    return szMsg;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

DUI_BASE_BEGIN_MESSAGE_MAP(CNotifyPump)
	DUI_END_MESSAGE_MAP()

	static const DUI_MSGMAP_ENTRY* DuiFindMessageEntry(const DUI_MSGMAP_ENTRY* lpEntry,TNotifyUI& msg )
{
	CDuiString sMsgType = msg.sType;
	CDuiString sCtrlName = msg.pSender->GetName();
	const DUI_MSGMAP_ENTRY* pMsgTypeEntry = NULL;
	while (lpEntry->nSig != DuiSig_end)
	{
		if(lpEntry->sMsgType==sMsgType)
		{
			if(!lpEntry->sCtrlName.IsEmpty())
			{
				if(lpEntry->sCtrlName==sCtrlName)
					return lpEntry;
			}
			else
				pMsgTypeEntry = lpEntry;
		}
		lpEntry++;
	}
	return pMsgTypeEntry;
}

bool CNotifyPump::AddVirtualWnd(CDuiString strName,CNotifyPump* pObject)
{
	if( m_VirtualWndMap.Find(strName) == NULL )
	{
		m_VirtualWndMap.Insert(strName.GetData(),(LPVOID)pObject);
		return true;
	}
	return false;
}

bool CNotifyPump::RemoveVirtualWnd(CDuiString strName)
{
	if( m_VirtualWndMap.Find(strName) != NULL )
	{
		m_VirtualWndMap.Remove(strName);
		return true;
	}
	return false;
}

bool CNotifyPump::LoopDispatch(TNotifyUI& msg)
{
	const DUI_MSGMAP_ENTRY* lpEntry = NULL;
	const DUI_MSGMAP* pMessageMap = NULL;

#ifndef UILIB_STATIC
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = pMessageMap->pBaseMap)
#endif
	{
#ifndef UILIB_STATIC
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
		ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif
		if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries,msg)) != NULL)
		{
			goto LDispatch;
		}
	}
	return false;

LDispatch:
	union DuiMessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	bool bRet = false;
	int nSig;
	nSig = lpEntry->nSig;
	switch (nSig)
	{
	default:
		ASSERT(FALSE);
		break;
	case DuiSig_lwl:
		(this->*mmf.pfn_Notify_lwl)(msg.wParam,msg.lParam);
		bRet = true;
		break;
	case DuiSig_vn:
		(this->*mmf.pfn_Notify_vn)(msg);
		bRet = true;
		break;
	}
	return bRet;
}

void CNotifyPump::NotifyPump(TNotifyUI& msg)
{
	///遍历虚拟窗口
	if( !msg.sVirtualWnd.IsEmpty() ){
		for( int i = 0; i< m_VirtualWndMap.GetSize(); i++ ) {
			if( LPCTSTR key = m_VirtualWndMap.GetAt(i) ) {
				if( _tcsicmp(key, msg.sVirtualWnd.GetData()) == 0 ){
					CNotifyPump* pObject = static_cast<CNotifyPump*>(m_VirtualWndMap.Find(key, false));
					if( pObject && pObject->LoopDispatch(msg) )
						return;
				}
			}
		}
	}

	///
	//遍历主窗口
	LoopDispatch( msg );
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CWindowWnd::CWindowWnd() : m_hWnd(NULL),m_hWndParent(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(false),m_bResizeable(true)
{
}

HWND CWindowWnd::GetHWND() const 
{ 
    return m_hWnd; 
}

HWND CWindowWnd::GetHWNDPARENT() const
{
	return m_hWndParent;
}

UINT CWindowWnd::GetClassStyle() const
{
    return 0;
}

LPCTSTR CWindowWnd::GetSuperClassName() const
{
    return NULL;
}

CWindowWnd::operator HWND() const
{
    return m_hWnd;
}

HWND CWindowWnd::CreateDuiWindow( HWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/ )
{
	return Create(hwndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0,NULL);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
    if( GetSuperClassName() != NULL && !RegisterSuperclass() ) return NULL;
    if( GetSuperClassName() == NULL && !RegisterWindowClass() ) return NULL;
    m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, CPaintManagerUI::GetInstance(), this);
    ASSERT(m_hWnd!=NULL);
	m_hWndParent = hwndParent;
    return m_hWnd;
}

HWND CWindowWnd::Subclass(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    ASSERT(m_hWnd==NULL);
    m_OldWndProc = SubclassWindow(hWnd, __WndProc);
    if( m_OldWndProc == NULL ) return NULL;
    m_bSubclassed = true;
    m_hWnd = hWnd;
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
    return m_hWnd;
}

void CWindowWnd::Unsubclass()
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    if( !m_bSubclassed ) return;
    SubclassWindow(m_hWnd, m_OldWndProc);
    m_OldWndProc = ::DefWindowProc;
    m_bSubclassed = false;
}

void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

UINT CWindowWnd::ShowModal()
{
    ASSERT(::IsWindow(m_hWnd));
    UINT nRet = 0;
	vector<WndState> vParentWnds;
	HWND hWndParent = GetParent(m_hWnd);
	HWND hWndAncestor = hWndParent;
	while(hWndAncestor && GetDesktopWindow() != hWndAncestor)
	{
		WndState ParentState;
		ParentState.hParentWnd = hWndAncestor;
		if(!IsWindowEnabled(hWndAncestor))
			ParentState.bEnable = false;
		vParentWnds.push_back(ParentState);
		hWndAncestor = GetParent(hWndAncestor);
	}
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	bool bNormalClose = false;
	for(unsigned int i = 0 ; i < vParentWnds.size() ; i++)
		::EnableWindow(vParentWnds[i].hParentWnd, FALSE);
    MSG msg = { 0 };
    while(IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) 
	{
        if( msg.message == WM_CLOSE ) 
         {
			 if(msg.hwnd == m_hWnd)
			 {
				 for(unsigned int i = 0 ; i < vParentWnds.size() ; i++)
					 ::EnableWindow(vParentWnds[i].hParentWnd, vParentWnds[i].bEnable);
				 bNormalClose = true;
				 ::SetFocus(hWndParent);
				 nRet = msg.wParam;
			 }
			 else if(msg.hwnd == hWndParent)
			 {
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				 break;
			 }
		}
        if( !CPaintManagerUI::TranslateMessage(&msg) ) 
		{
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        if( msg.message == WM_QUIT ) break;
    }
	if(!bNormalClose)
	{
		if(m_hWnd&&IsWindow(m_hWnd))
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}
		for(unsigned int i = 0 ; i < vParentWnds.size() ; i++)
			::EnableWindow(vParentWnds[i].hParentWnd, vParentWnds[i].bEnable);
		::SetFocus(hWndParent);
	}
    if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
    return nRet;
}

void CWindowWnd::Close(UINT nRet)
{
    if( !::IsWindow(m_hWnd) ) return;
    PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
}

void CWindowWnd::CenterWindow( DWORD nMonitorIndex /*= 0*/ )
{
	DWORD mMonitorIndex = nMonitorIndex;
	if(DuiMonitor::GetMonitorCount() < nMonitorIndex || (nMonitorIndex == 0 && GetHWNDPARENT() == NULL))
		nMonitorIndex = 1;

	HWND mParentHwnd = GetHWNDPARENT();
	RECT mParentWndRect;
	GetWindowRect(mParentHwnd,&mParentWndRect);
	INT mParentWndHeight = mParentWndRect.bottom - mParentWndRect.top;
	INT mParentWndWidth	 = mParentWndRect.right - mParentWndRect.left;

	if(!mParentHwnd)
	{
		SIZE mMonitor = DuiMonitor::GetMonitorSize(nMonitorIndex);
		POINTL mPoint = DuiMonitor::GetMonitorXY(nMonitorIndex);
		
		mParentWndRect.left		= mPoint.x;
		mParentWndRect.top		= mPoint.y;
		mParentWndRect.right	= mMonitor.cx;
		mParentWndRect.bottom	= mMonitor.cy;

		mParentWndWidth			= mMonitor.cx;
		mParentWndHeight		= mMonitor.cy;
	}

	RECT mCurWndRect;
	GetWindowRect(m_hWnd,&mCurWndRect);
	INT mCurWndHeight	= mCurWndRect.bottom - mCurWndRect.top;
	INT mCurWndWidth	= mCurWndRect.right - mCurWndRect.left;

	LONG styleValue = ::GetWindowLongPtr(*this, GWL_STYLE);
	if((styleValue&WS_CHILD)==WS_CHILD)
		mParentWndRect.top = mParentWndRect.left = 0;

	mCurWndRect.top		= LONG(mParentWndRect.top + (mParentWndHeight/2 - mCurWndHeight/2));
	mCurWndRect.left	= LONG(mParentWndRect.left + (mParentWndWidth/2 - mCurWndWidth/2));

	::SetWindowPos(m_hWnd,NULL,mCurWndRect.left,mCurWndRect.top,0,0,SWP_NOSIZE | SWP_NOZORDER);
}

void CWindowWnd::SetIcon(UINT nRes)
{
		HINSTANCE HResInstance = CPaintManagerUI::GetResourceDll();
		HICON hIcon = (HICON)::LoadImage(HResInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// In case it get blur in Hight DPI
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);

		hIcon = (HICON)::LoadImage(HResInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

void CWindowWnd::SetWindowResizeable(bool bResizeable)
{
	m_bResizeable = bResizeable;
	DWORD dwStyle =GetWindowLongPtr(m_hWnd,GWL_STYLE);
	if(dwStyle & WS_MAXIMIZEBOX)
	{
		if(!m_bResizeable)
			dwStyle &= ~ WS_MAXIMIZEBOX;
	}
	else
	{
		if(m_bResizeable)
			dwStyle &= WS_MAXIMIZEBOX;
	}
	::SetWindowLongPtr(m_hWnd,GWL_STYLE,dwStyle);
}


bool CWindowWnd::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CWindowWnd::__WndProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool CWindowWnd::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) ) {
        if( !::GetClassInfoEx(CPaintManagerUI::GetInstance(), GetSuperClassName(), &wc) ) {
            ASSERT(!"Unable to locate window class");
            return NULL;
        }
    }
    m_OldWndProc = wc.lpfnWndProc;
    wc.lpfnWndProc = CWindowWnd::__ControlProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
		if(pThis)
		{
			pThis->m_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
		}
    } 
    else {
        pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL  ) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
		if(pThis)
		{
			::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
			pThis->m_hWnd = hWnd;
		}
    } 
    else {
        pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, _T("WndX")));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            ::SetProp(hWnd, _T("WndX"), NULL);
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL ) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
    ASSERT(::IsWindow(m_hWnd));
    RECT rc = { 0 };
    if( !::GetClientRect(m_hWnd, &rc) ) return;
    if( cx != -1 ) rc.right = cx;
    if( cy != -1 ) rc.bottom = cy;
    if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
    ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void CWindowWnd::CustomDraw(HDC memDC)
{
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(IsWindow(m_hWnd))
		return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	return 0;
}

void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
{
}

void CWindowWnd::EndModal()
{
	::DestroyWindow(m_hWnd);
	m_hWnd = NULL;
}

} // namespace UiLib
