#include "StdAfx.h"
#include "CommonPacketMaker.h"
#include "MainWnd.h"

CMainWnd::CMainWnd(void)
{
	m_pLblFileName = NULL;
	m_pEdtPwd = NULL;
	m_pBtnEnCrypt = NULL;
	m_pBtnDeCrypt = NULL;
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
	return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\"));
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("ZIPWND");
}

LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		break;
	}
 	return IWindowBase::HandleMessage(uMsg,wParam,lParam);
}

void CMainWnd::Init()
{

	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(*this, GWL_HINSTANCE);
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	SetClassLong(*this, GCL_HICON, (LONG)hIcon);

	DragAcceptFiles(m_hWnd,TRUE);
#ifndef _DEBUG 
	AllowMeesageForVistaAbove(WM_DROPFILES, MSGFLT_ADD);
#endif
	IWindowBase::Init();
}

void CMainWnd::Notify( TNotifyUI& msg )
{
	if(msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnPrepare();
	IWindowBase::Notify(msg);
}

void CMainWnd::OnFinalMessage( HWND hWnd )
{
	IWindowBase::OnFinalMessage(hWnd);
	PostQuitMessage(0);
	delete this;
}

UILIB_RESOURCETYPE CMainWnd::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR CMainWnd::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_ZIPRES1);
}

char * CMainWnd::GetResourcePwd() const
{
	return NULL;
}

void CMainWnd::OnPrepare()
{
	m_pLblFileName = static_cast<CLabelUI *>(pm.FindControl(_T("LblFileName")));
	m_pEdtPwd = static_cast<CEditUI *>(pm.FindControl(_T("EdtPwd")));
	m_pBtnEnCrypt = static_cast<CButtonUI *>(pm.FindControl(_T("BtnEnCrypt")));
	m_pBtnDeCrypt = static_cast<CButtonUI *>(pm.FindControl(_T("BtnDeCrypt")));

	if(m_pBtnEnCrypt)
		m_pBtnEnCrypt->OnNotify += MakeDelegate(this, &CMainWnd::OnBtnEnCrypt, DUI_MSGTYPE_CLICK);
	if(m_pBtnDeCrypt)
		m_pBtnDeCrypt->OnNotify += MakeDelegate(this, &CMainWnd::OnBtnDeCrypt, DUI_MSGTYPE_CLICK);
}

bool CMainWnd::OnBtnEnCrypt( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	if(m_szFilePath.IsEmpty())
	{
		MessageBox(m_hWnd,_T("请先加载文件"),_T("ERROR"),MB_OK);
		return true;
	}
	CString szPwd;
	if(m_pEdtPwd)
		szPwd = m_pEdtPwd->GetText();
	CCommonPacketMaker::EncryptPack(m_szFilePath,szPwd);
	return true;
}

bool CMainWnd::OnBtnDeCrypt( TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam )
{
	if(m_szFilePath.IsEmpty())
	{
		MessageBox(m_hWnd,_T("请先加载文件"),_T("ERROR"),MB_OK);
		return true;
	}
	CString szPwd;
	if(m_pEdtPwd)
		szPwd = m_pEdtPwd->GetText();
	CCommonPacketMaker::DecryptPack(m_szFilePath,szPwd);
	return true;
}

BOOL CMainWnd::AllowMeesageForVistaAbove( UINT uMessageID, BOOL bAllow )
{
	BOOL bResult = FALSE;
	HMODULE hUserMod = NULL;
	hUserMod = LoadLibrary(_T("user32.dll"));
	if(!hUserMod ) return FALSE;
	_ChangeWindowMessageFilter pChangeWindowMessageFilter = (_ChangeWindowMessageFilter)GetProcAddress( hUserMod, "ChangeWindowMessageFilter");
	if(!pChangeWindowMessageFilter ) return FALSE;
	bResult = pChangeWindowMessageFilter( uMessageID, bAllow ? 1 : 2 );
	if(!hUserMod ) FreeLibrary( hUserMod );
	return bResult;
}

void CMainWnd::OnDropFiles( HDROP hDropInfo )
{
	int nNumb = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);
	if(nNumb > 1)
		MessageBox(m_hWnd,_T("文件类型不匹配"),_T("Error"),MB_OK);
	else
	{
		TCHAR szpathName[MAX_PATH];
		DWORD dwSize = DragQueryFile(hDropInfo,0,NULL,0);
		DragQueryFile(hDropInfo,0,szpathName,dwSize + 1);
		m_szFilePath = szpathName;
		if(!PathIsDirectory(m_szFilePath)&&m_szFilePath.Find(_T(".zip")) == -1)
		{
			m_szFilePath = _T("");
			MessageBox(m_hWnd,_T("文件类型不匹配"),_T("Error"),MB_OK);
		}
		else if(m_pLblFileName)
		{
			CString szFileName = PathFindFileName(m_szFilePath);
			m_pLblFileName->SetText(szFileName);
		}
	}
}
