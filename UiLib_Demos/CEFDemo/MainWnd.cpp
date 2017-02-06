#include "StdAfx.h"
#include "MainWnd.h"

CMainWnd::CMainWnd(void)
{
	
} 

CMainWnd::~CMainWnd(void)
{
}

CDuiString CMainWnd::GetSkinFile()
{
	return CDuiString(_T("MainSkin.xml"));
}

CDuiString CMainWnd::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\CEFTest\\"));
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("CEFDemo");
}

LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
 	return IWindowBase::HandleMessage(uMsg,wParam,lParam);
}

void CMainWnd::Init()
{
	IWindowBase::Init();
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
