#include "StdAfx.h"
#include "MainWnd.h"
#include "MutualDef.h"

HHOOK CMainWnd::m_hChromMouseHOOK = NULL;
HWND CMainWnd::m_hParent = NULL;

CMainWnd::CMainWnd(void)
{
	m_pChrome = NULL;
} 

CMainWnd::~CMainWnd(void)
{
	if(m_hChromMouseHOOK)
	{
		UnhookWindowsHookEx(m_hChromMouseHOOK);
		m_hChromMouseHOOK = NULL;
	}
}

CDuiString CMainWnd::GetSkinFile()
{
	return CDuiString(_T("MainSkin.xml"));
}

CDuiString CMainWnd::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\MultiChrome\\"));
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("MultiChrome");
}

LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#pragma warning(disable:4800)
	if(uMsg == WM_CEF_CREATE&&m_hWndParent)
	{
		if(m_pChrome)
		{
			::SendMessage(m_hWndParent,WM_CEF_CREATE,WPARAM(m_hWnd),m_pChrome->GetUIThreadID());
			HookMouseProc();
		}
	}
	else if(uMsg == WM_SHOWCHROME)
		ShowWindow((bool)wParam,false);
	else if(uMsg == WM_MOVECHROME)
	{
		int nY = HIWORD(wParam);
		int nX = LOWORD(wParam);
		int nHeight = HIWORD(lParam);
		int nWidth = LOWORD(lParam);
		MoveWindow(m_hWnd,nX,nY,nWidth,nHeight,TRUE);
	}
	else if(uMsg == WM_COPYDATA)
	{
		COPYDATASTRUCT *pCpD = (COPYDATASTRUCT *)lParam;
		CString szURL;
		szURL.Format(_T("%s"),(LPSTR)(pCpD->lpData));
		if(m_pChrome)
			m_pChrome->SetHomePage(szURL);
	}
	return IWindowBase::HandleMessage(uMsg,wParam,lParam);
}

void CMainWnd::Init()
{
	IWindowBase::Init();
	m_pChrome = static_cast<CChromeUI *>(pm.FindSubControlByClass(NULL,_T("ChromeUI")));
}


void CMainWnd::Notify( TNotifyUI& msg )
{
	IWindowBase::Notify(msg);
}

void CMainWnd::OnFinalMessage( HWND hWnd )
{
	IWindowBase::OnFinalMessage(hWnd);
	PostQuitMessage(0);
	delete this;
}

LRESULT CALLBACK ChromeUIMouseProc(_In_  int nCode,_In_  WPARAM wParam,_In_  LPARAM lParam)
{
	if (nCode < 0 || nCode == HC_NOREMOVE)
	{
		return ::CallNextHookEx(CMainWnd::m_hChromMouseHOOK, nCode, wParam, lParam);
	}
	if (wParam == WM_LBUTTONDOWN || wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONUP)
	{
		MOUSEHOOKSTRUCT* pMoushook = (MOUSEHOOKSTRUCT*)lParam;
		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = sizeof(ChromeMouseMsg);
		ChromeMouseMsg *pChromeMsg = new ChromeMouseMsg;
		pChromeMsg->wParam = wParam;
		pChromeMsg->lParam = MAKELPARAM(pMoushook->pt.x,pMoushook->pt.y);;
		cpd.lpData = (void*)pChromeMsg;
		::SendMessage(CMainWnd::m_hParent,WM_COPYDATA,0,(LPARAM)&cpd);
		SAFE_DELETE(pChromeMsg);
	}
	return  ::CallNextHookEx(CMainWnd::m_hChromMouseHOOK,nCode,wParam,lParam);
}

void CMainWnd::HookMouseProc()
{
	if(m_pChrome)
		m_hChromMouseHOOK = ::SetWindowsHookEx(WH_MOUSE,ChromeUIMouseProc,NULL,m_pChrome->GetUIThreadID());
}