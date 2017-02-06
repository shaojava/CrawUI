#include "StdAfx.h"
#include "MainWnd.h"

CMainWnd::CMainWnd(void)
{
	m_pToolTipTest = NULL;
	m_pBtnNewChrome = NULL;
	m_pAnimate = NULL;
	m_pReload = NULL;

	m_pBtn1 = NULL;
	m_pBtn2 = NULL;
	m_pBtn3 = NULL;
	m_pBtn4 = NULL;
	m_pBtn5 = NULL;
	m_pMask = NULL;

	m_pItme1 = NULL;
	m_pItme2 = NULL;
	m_pItme3 = NULL;
	m_pItme4 = NULL;
	m_pItme5 = NULL;
	m_pItme6 = NULL;

	m_bPause = false;
	m_pChrome = NULL;

	m_pList = NULL;

	m_pMachineView = NULL;

	m_pEdit = NULL;

	m_pCombo = NULL;
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
	return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\ControlTest\\"));
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("Control_Demos");
}

CFrameIamgeUI *g_pFrameImage = NULL;

LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	
	if(uMsg == WM_MOUSEWHEEL)
	{
// 		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
// 		::ScreenToClient(m_hWnd, &pt);
// 		int zDelta = (int) (short) HIWORD(wParam);
// 		TEventUI event = { 0 };
// 		event.Type = UIEVENT_SCROLLWHEEL;
// 		event.pSender = NULL;
// 		event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, zDelta);
// 		event.lParam = lParam;
// 		event.wKeyState = MapKeyState();
// 		event.dwTimestamp = ::GetTickCount();
// 		m_pAnimate->Event(event);
	}
	else if(uMsg == WM_CHAR)
	{
		if(wParam == 'p')
		{
			int i,j,k;
			m_pList->GetSnapBmp(i,j,k,true);
			m_pList->Invalidate();
			TRACE(_T("~~~~~~~~"));
// 			m_bPause = !m_bPause;
// 			m_pAnimate->Pause(m_bPause);
		}
		if(wParam == 'o')
		{
			if(m_pMachineView)
				m_pMachineView->AddItem();
		}
		else if(wParam == 'n')
		{
			CMainWnd *pWnd = new CMainWnd;
			pWnd->Create(NULL,NULL,WS_POPUPWINDOW|WS_VISIBLE,0);
		}
		else if(wParam == 'g')
		{
			int nWidth,nHeight,nPitch;
			CContainerUI *pContainer = static_cast<CContainerUI *>(pm.FindControl(_T("parent")));
			HBITMAP bitmap = pContainer->GetSnapBmp(nWidth,nHeight,nPitch,true);
		}
	}
	else if(uMsg == WM_COPYDATA)
	{
		COPYDATASTRUCT *pCpD = (COPYDATASTRUCT *)lParam;
		ChromeMouseMsg *pMsg = (ChromeMouseMsg *)pCpD->lpData;
		UINT uMsg = (UINT)pMsg->wParam;
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			{
				int a = 0;
			}
			break;
		default:
			break;
		}
	}
 	return IWindowBase::HandleMessage(uMsg,wParam,lParam);
}

void CMainWnd::Init()
{
	IWindowBase::Init();

	LPCTSTR szPath = pm.GetRoot()->GetBkImage();

	m_pToolTipTest = static_cast<CButtonUI *>(pm.FindControl(_T("ToolTip")));
	m_pReload = static_cast<CButtonUI *>(pm.FindControl(_T("reload")));
	m_pAnimate = static_cast<CAnimateTabUI *>(pm.FindControl(_T("animate")));

	m_pItme1 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme1")));
	m_pItme2 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme2")));
	m_pItme3 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme3")));
	m_pItme4 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme4")));
	m_pItme5 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme5")));
	m_pItme6 = static_cast<CHorizontalLayoutUI *>(pm.FindControl(_T("itme6")));

	m_pBtn1 = static_cast<CButtonUI *>(pm.FindControl(_T("shortcut")));
	m_pBtn2 = static_cast<CButtonUI *>(pm.FindControl(_T("btn2")));
	m_pBtn3 = static_cast<CButtonUI *>(pm.FindControl(_T("btn3")));
	m_pBtn4 = static_cast<CButtonUI *>(pm.FindControl(_T("btn4")));
	m_pBtn5 = static_cast<CButtonUI *>(pm.FindControl(_T("btn5")));
	m_pBtnNewChrome = static_cast<CButtonUI *>(pm.FindControl(_T("newchrome")));

	m_pChrome = static_cast<CMltPrsChromUI *>(pm.FindControl(_T("chrome")));
	m_pList = static_cast<CListUI*>(pm.FindControl(_T("domainlist")));

	m_pMask = static_cast<CMaskControlUI *>(pm.FindControl(_T("focus")));

	m_pCombo = static_cast<CComboUI *>(pm.FindControl(_T("CmbOperator")));
	if(m_pCombo)
	{
		for(int i = 0 ; i < 3; i++)
		{
			CListLabelElementUI *pListItem = new CListLabelElementUI;
			pListItem->SetFixedHeight(25);
			CString szTmp;
			szTmp.Format(_T("ÊÕÒøÔ±%d"),i);
			pListItem->SetText(szTmp);
			m_pCombo->Add(pListItem);
		}
		m_pCombo->SelectItem(0);
	}
	if(m_pMask)
		m_pMask->SetMaskTarget(m_pBtn1);

	m_pEdit = static_cast<CEditUI*>(pm.FindControl(_T("edit")));


	g_pFrameImage =  static_cast<CFrameIamgeUI *>(pm.FindControl(_T("frame")));
	Image *pImage1 = new Image(L"C:\\pics\\1.jpg");
	g_pFrameImage->AddFrame(pImage1);
	Image *pImage2 = new Image(L"C:\\pics\\2.jpg");
	g_pFrameImage->AddFrame(pImage2);
	Image *pImage3 = new Image(L"C:\\pics\\4.jpg");
	g_pFrameImage->AddFrame(pImage3);
	Image *pImage4 = new Image(L"C:\\pics\\6.jpg");
	g_pFrameImage->AddFrame(pImage4);
	Image *pImage5 = new Image(L"C:\\pics\\test.jpg");
	g_pFrameImage->AddFrame(pImage5);
	g_pFrameImage->StartPlay(1000);

	

}


BOOL  CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{      
	if(hwnd)
	{
		unsigned long pid;
		GetWindowThreadProcessId(hwnd, &pid );
		unsigned long *p = (unsigned long*)lParam;
		if ( pid == *p && GetParent(hwnd) == NULL )
		{
			*p = (unsigned long)hwnd;
			return FALSE;
		}
	}
	return TRUE;
}

BOOL GetMainHwndByPid(unsigned long pid, HWND *hwnd)
{
	*hwnd = (HWND)pid;
	EnumWindows((WNDENUMPROC)lpEnumFunc, (LPARAM)hwnd);
	return IsWindow(*hwnd);
}

bool cmp1(CListElementUI *p1,CListElementUI *p2)
{
	CListTextElementUI *t1 = (CListTextElementUI *)p1;
	CListTextElementUI *t2 = (CListTextElementUI *)p2;
	CDuiString sz1(t1->GetText(0));
	CDuiString sz2(t2->GetText(0));
	return  sz1 > sz2;
}

void CMainWnd::Notify( TNotifyUI& msg )
{
	if(_tcsicmp(msg.sType,_T("windowinit")) == 0)
	{
		//StartUnLayeredShadow();

		
	}
	else if(_tcsicmp(msg.sType,_T("click")) == 0)
	{
		if(msg.pSender->GetName() == _T("OptOverHour"))
		{
			CTabLayoutUI *pTab = static_cast<CTabLayoutUI *>(pm.FindControl(_T("ChargeDisplayPanel")));
			pTab->SelectItem(1);
		}
		else if(msg.pSender->GetName() == _T("OptUnitHour"))
		{
			CTabLayoutUI *pTab = static_cast<CTabLayoutUI *>(pm.FindControl(_T("ChargeDisplayPanel")));
			pTab->SelectItem(0);
		}
		else if (msg.pSender->GetName() == _T("ModifyHeight"))
		{
			CVerticalLayoutUI *pParent = (CVerticalLayoutUI*)(msg.pSender->GetParent());
			if(pParent->GetFixedHeight() == 600)
				pParent->SetFixedHeight(-1);
			else
				pParent->SetFixedHeight(450);

			CAboutWnd *pWnd = new CAboutWnd;
			pWnd->Create(m_hWnd, _T("UiLib Demos"), UI_WNDSTYLE_FRAME,0,0,500,600);
			pWnd->CenterWindow();
			pWnd->ShowWindow();
		}
		else if (msg.pSender->GetName() == _T("AddList"))
		{
			//for(int i = 0; i < 20 ; i++)
			{
				CListTextElementUI* pListElement = new CListTextElementUI;
				CDuiString szNum;
				szNum.Format(_T("%d"),m_pList->GetCount());

				if(m_pList->GetCount() < 4)
					m_pList->AddAt(pListElement,0);
				else
					m_pList->AddAt(pListElement,3);
				pListElement->SetFixedHeight(25);
				pListElement->SetName(szNum);
				pListElement->SetText(0,szNum);
				pListElement->SetText(1,_T("wwf"));
				pListElement->SetText(2,_T("fewfew"));
			}
		}
		else if (msg.pSender->GetName() == _T("Memory"))
		{
			if(m_pMachineView)
			{
				for(int i = 0 ; i < 829 ; i++)
					m_pMachineView->AddItem();
			}
			
			::SendMessage(m_hWnd,WM_PAINT,NULL,NULL);

		}
		else if(msg.pSender->GetName() == _T("DeleteList"))
		{
			if( m_pList ) 
				m_pList->RemoveSelectItems();
			if(m_pMachineView)
				m_pMachineView->RemoveAll();

		}
		else if(msg.pSender->GetName() == _T("Sort"))
		{
			if(m_pList)
				m_pList->SortItems((CompareFunc)cmp1);
		}
		else if(msg.pSender == m_pToolTipTest)
		{
			pToolTiWnd = CToolTipsWnd::CreateToolTipsWnd(m_hWnd);
			SIZE szWnd = {250,250};
			POINT ptMouse={msg.ptMouse.x,msg.ptMouse.y};
			ClientToScreen(m_hWnd,&ptMouse);
			
			pToolTiWnd->Attach(NULL,szWnd,ptMouse);
			pToolTiWnd->SetTipString(_T("fdsafdsa"));
		}
		else if(msg.pSender == m_pReload)
		{
			CPaintManagerUI::ReloadSkin();
		}
		else if(msg.pSender == m_pBtn1)
		{
			HDC dcWnd = GetDC(NULL);
			HBITMAP bmp = CreateCompatibleBitmap(dcWnd,900,600);
			HDC hdc = CreateCompatibleDC(dcWnd);
			SelectObject(hdc,bmp);
			BitBlt(hdc,0,0,900,600,GetDC(NULL),510,300,SRCCOPY);
			Graphics gs(GetDC(NULL));
			Bitmap *pBitmap = Bitmap::FromHBITMAP(bmp,NULL);
			gs.DrawImage(pBitmap,0,0);
		}
		else if(msg.pSender == m_pBtn2)
		{
			RECT rcMask = m_pBtn2->GetPos();
			if(m_pMask)
			{
				m_pMask->SetMaskTarget(m_pBtn2);
				m_pMask->AnimateTo(rcMask);
			}
		
		}
		else if(msg.pSender == m_pBtn3)
		{
			RECT rcMask = m_pBtn3->GetPos();
			if(m_pMask)
			{
				m_pMask->SetMaskTarget(m_pBtn3);
				m_pMask->AnimateTo(rcMask);
			}
		}
		else if(msg.pSender == m_pBtn4)
		{
			RECT rcMask = m_pBtn4->GetPos();
			if(m_pMask)
			{
				m_pMask->SetMaskTarget(m_pBtn4);
				m_pMask->AnimateTo(rcMask);
			}
		}
		else if(msg.pSender == m_pBtn5)
		{
			RECT rcMask = m_pBtn5->GetPos();
			if(m_pMask)
			{
				m_pMask->SetMaskTarget(m_pBtn5);
				m_pMask->AnimateTo(rcMask);
			}
		}
		else if(msg.pSender == m_pBtnNewChrome)
		{
			
			PROCESS_INFORMATION pi;

			LPSECURITY_ATTRIBUTES lpAtt = new SECURITY_ATTRIBUTES;
			lpAtt->bInheritHandle = TRUE;
			lpAtt->lpSecurityDescriptor = NULL;
			lpAtt->nLength = sizeof(SECURITY_ATTRIBUTES);

			STARTUPINFO si = { sizeof(si) }; 
			si.dwFlags = STARTF_USESHOWWINDOW; 
			si.wShowWindow = TRUE;
			CDuiString szPath = CPaintManagerUI::GetInstancePath();
			szPath += _T("MultiProcessChrome.exe");
			
			 TCHAR szAppPath[MAX_PATH] = {0};

			 _stprintf(szAppPath, _T("%s 0x%x"),_T("100,100,300,300"),m_hWnd);

			BOOL bRet = ::CreateProcess (
				szPath,
				szAppPath,
				lpAtt,
				NULL,
				TRUE,
				NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&si,
				&pi);
			::CloseHandle (pi.hThread); 
			::CloseHandle (pi.hProcess);
			SAFE_DELETE(lpAtt);
		}
	}
	else if( msg.sType == _T("showactivex") ) 
	{
		if( msg.pSender->GetName() != _T("flash") ) return;
		IShockwaveFlash* pFlash = NULL;
		CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);
		if (pActiveX)
		{
			pActiveX->GetControl(IID_IUnknown, (void**)&pFlash);
			if( pFlash != NULL ) 
			{
				pFlash->PutAllowScriptAccess(L"always");
				pFlash->PutWMode(L"transparent");
				pFlash->PutSAlign(L"middle");
				pFlash->PutScale((_bstr_t(_T("showAll"))));
				HRESULT hr;
				pFlash->PutEmbedMovie(TRUE);
				CDuiString str = CPaintManagerUI::GetInstancePath() + _T("Skins\\ControlTest\\ico.swf");
				hr = pFlash->LoadMovie(0,  _bstr_t(str));
				hr = pFlash->Play();
				pFlash->Release();
			}  
		}

	}
	else if( msg.sType == _T("menu") ) 
	{
		CMenuWnd *pMenu = CMenuWnd::CreateDuiMenu(m_hWnd);
		pMenu->SetNotifier(this);
		POINT ptMoust = msg.ptMouse;
		ClientToScreen(m_hWnd, &ptMoust);
		pMenu->Init(NULL,_T("menutest.xml"),0, ptMoust);
	}
	else if(msg.sType == _T("menuclose"))
	{
		CMenuElementUI *pMenu = (CMenuElementUI *)(msg.pSender->GetInterface(DUI_CTR_MENUELEMENT));
		if(pMenu)
		{
			CDuiString szLocation = pMenu->GetLocation();
		}
	}
	IWindowBase::Notify(msg);
}

void CMainWnd::OnFinalMessage( HWND hWnd )
{
	IWindowBase::OnFinalMessage(hWnd);
	PostQuitMessage(0);
	delete this;
}

CControlUI * CMainWnd::CreateControl(LPCTSTR pstrClass,CPaintManagerUI *pManager)
{
	if(_tcsicmp(pstrClass,_T("WallPaper")) == 0)
	{
		CWallPaperUI *p = new CWallPaperUI(pManager);
		return p;
	}
	else if(_tcsicmp(pstrClass,_T("AnimateTab")) == 0)
	{
		CAnimateTabUI *p = new CAnimateTabUI();
		return p;
	}
// 	else if(_tcsicmp(pstrClass,_T("MltPrsChrom")) == 0)
// 	{
// 		CMltPrsChromUI *p = new CMltPrsChromUI();
// 		return p;
// 	}
	if (_tcsicmp(pstrClass, _T("MachineTree")) == 0)
	{
		m_pMachineView = new CMachineTree();
		return (CControlUI *)m_pMachineView;
	}
	else if (_tcsicmp(pstrClass, _T("FrameImage")) == 0)
	{
		CFrameIamgeUI *pFrame = new CFrameIamgeUI;
		return (CControlUI *)pFrame;
	}
	return NULL;
}
