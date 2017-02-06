#include "UIMltPrsChrom.h"

CMltPrsChromUI::CMltPrsChromUI(void)
{
	m_hWndAttached = NULL;
	m_dwProcessID = 0;
}

CMltPrsChromUI::~CMltPrsChromUI(void)
{
}

BOOL CMltPrsChromUI::Attach( HWND hWndNew )
{
	if (!::IsWindow(hWndNew))
		return FALSE;
	m_hWndAttached = hWndNew;
	return TRUE;
}

HWND CMltPrsChromUI::Detach()
{
	HWND hWnd = m_hWndAttached;
	m_hWndAttached = NULL;
	return hWnd;
}

HWND CMltPrsChromUI::GetHWND()
{
	return m_hWndAttached;
}

void CMltPrsChromUI::SetPos( RECT rc )
{
	__super::SetPos(rc);
	if(m_hWndAttached)
	{
		
		if(!IsWindowVisible(m_hWndAttached))
			::SendMessage(m_hWndAttached,WM_SHOWCHROME,(WPARAM)TRUE,NULL);
		WPARAM wParam = MAKEWPARAM(rc.left,rc.top);
		LPARAM lParam = MAKELPARAM(rc.right-rc.left,rc.bottom-rc.top);
		::SendMessageA(m_hWndAttached,WM_MOVECHROME,wParam,lParam);
	}
	else
	{
		if(!m_dwProcessID)
			CreateChrome(rc);
	}
}

void CMltPrsChromUI::SetInternVisible( bool bVisible /*= true*/ )
{
	__super::SetInternVisible(bVisible);
	if(m_hWndAttached)
		::SendMessage(m_hWndAttached,WM_SHOWCHROME,(WPARAM)bVisible,NULL);
}

void CMltPrsChromUI::SetVisible( bool bVisible /*= true*/ )
{
	__super::SetVisible(bVisible);
	if(m_hWndAttached)
		::SendMessage(m_hWndAttached,WM_SHOWCHROME,(WPARAM)bVisible,NULL);
}

void CMltPrsChromUI::CreateChrome(RECT rcPos)
{
	PROCESS_INFORMATION pi;

	SECURITY_ATTRIBUTES Att;
	Att.bInheritHandle = TRUE;
	Att.lpSecurityDescriptor = NULL;
	Att.nLength = sizeof(SECURITY_ATTRIBUTES);

	STARTUPINFO si = { sizeof(si) }; 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = TRUE;
	CDuiString szPath = CPaintManagerUI::GetInstancePath();
	szPath += _T("MultiProcessChrome.exe");

	TCHAR szAppPath[MAX_PATH] = {0};

	HWND hParent = m_pManager->GetPaintWindow();
	CString szPos;
	szPos.Format(_T("%d,%d,%d,%d 0x%x"),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,hParent);
	_stprintf(szAppPath, _T("%s"),szPos);
	BOOL bRet = ::CreateProcess (
		szPath,
		szAppPath,
		&Att,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi);
	m_dwProcessID = pi.dwProcessId;
	::CloseHandle (pi.hThread); 
	::CloseHandle (pi.hProcess);
}

void CMltPrsChromUI::SetHomePage( LPCTSTR lpszUrl )
{
	if(m_hWndAttached)
	{
		CString sCopyData(lpszUrl);
		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = (sCopyData.GetLength() + 1)*2;
		cpd.lpData = (void*)sCopyData.GetBuffer(cpd.cbData);
		::SendMessage(m_hWndAttached,WM_COPYDATA,0,(LPARAM)&cpd);
	}
}