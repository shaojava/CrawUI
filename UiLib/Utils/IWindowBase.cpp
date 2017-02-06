#include "StdAfx.h"
#include "IWindowBase.h"



IWindowBase::IWindowBase(LPCTSTR _ZipSkin /*= NULL*/)
{
	m_pWndShadow = NULL;
	m_hDwmAPI = NULL;
	mpCloseBtn		= NULL;
	mpMaxBtn		= NULL;
	mpRestoreBtn	= NULL;
	mpMinBtn		= NULL;
	m_lpResourceZIPBuffer = NULL;
	ZipSkinPath		= _ZipSkin?_ZipSkin:_T("");
}

IWindowBase::~IWindowBase()
{
	if(m_lpResourceZIPBuffer)
	{
		delete[] m_lpResourceZIPBuffer;
		m_lpResourceZIPBuffer = NULL;
	}
	if(m_hDwmAPI)
	{
		FreeLibrary(m_hDwmAPI);
		m_hDwmAPI = NULL;
	}
}

LPCTSTR IWindowBase::GetWindowClassName() const
{
	try
	{
		return _T("WindowClassName");
	}
	catch(...)
	{
		throw "IWindowBase::GetWindowClassName";
	}
}

CControlUI* IWindowBase::CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager)
{
	return NULL;
}

CControlUI* IWindowBase::CreateControl( LPCTSTR pstrClass )
{
	try
	{
		return NULL;
	}
	catch(...)
	{
		throw "IWindowBase::CreateControl";
	}
}

void IWindowBase::OnFinalMessage(HWND hWnd)
{
	pm.RemovePreMessageFilter(this);
	pm.ReapObjects(pm.GetRoot());
	pm.RemoveNotifier(this);
	pm.GetTrayObject().DeleteTrayIcon();

	HWND hWndParent = GetParent(hWnd);
	if(hWndParent)
		::SendMessage(hWndParent,WM_USER + 2,NULL,NULL);

	if(m_lpResourceZIPBuffer)
	{
		int nID = (int)GetResourceID();
		CDuiString szCurResID;
		szCurResID.Format(_T("RESID:%d"),nID);
		if(szCurResID == CPaintManagerUI::GetResourceZip())
			CPaintManagerUI::Term();
	}

	SAFE_DELETE(m_pWndShadow);
}

void IWindowBase::Init()
{

}

LRESULT IWindowBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(m_pWndShadow)
		m_pWndShadow->Hide();
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT IWindowBase::OnNcCalcSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT IWindowBase::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT IWindowBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) && m_bResizeable) {
		RECT rcSizeBox = pm.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(pm.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("FadeButtonUI")) != 0 )
				return HTCAPTION;
	}


	if (-1 == rcCaption.bottom)
	{
		rcCaption.bottom = rcClient.bottom;
	}

	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = pm.FindControl(pt);
		if (IsInStaticControl(pControl))
			return HTCAPTION;
	}


	return HTCLIENT;
}

LRESULT IWindowBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.GetWidth();
	lpMMI->ptMaxSize.y		= rcWork.GetHeight();

	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(wParam == VK_RETURN)
	{
		if(pm.HandleIDOKEvent())
			bHandled = TRUE;
	}
	return 0;
}

LRESULT IWindowBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT IWindowBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = pm.GetRoundCorner();

	if( !::IsIconic(*this)) 
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		if(szRoundCorner.cx || szRoundCorner.cy)
		{
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
		else if(m_pWndShadow)
		{
			HRGN hRgn = ::CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
	}

	bHandled = FALSE;
	return 0;
}


LRESULT IWindowBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	if(wParam == SC_RESTORE && !IsWindowEnabled(m_hWnd))
		::ShowWindow(m_hWnd,TRUE);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if(wParam != SC_CLOSE)
	{
		if(::IsZoomed(m_hWnd) != bZoomed)
		{
			if( !bZoomed ) {
				if( mpMaxBtn ) mpMaxBtn->SetVisible(false);
				if( mpRestoreBtn ) mpRestoreBtn->SetVisible(true);
			}
			else {
				if( mpMaxBtn ) mpMaxBtn->SetVisible(true);
				if( mpRestoreBtn ) mpRestoreBtn->SetVisible(false);
			}
		}
	}
	return lRes;
}

void IWindowBase::ChooseLanuage()
{
	CDuiString szXmlPath = GetMLStringsPath();
	if(!szXmlPath.IsEmpty())
	{
		pm.SetUseMutiLanguage();
		CDialogBuilder builder;
		builder.ParseMultiLanguage(szXmlPath,&pm);
	}
}

CDuiString IWindowBase::GetMLStringsPath()
{
	return _T("");
}

CDuiString IWindowBase::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()) + _T("Skins\\Default\\");
}

void IWindowBase::StartGlassEffect(BOOL bOn)
{
	if(bOn)
	{
		if(!m_hDwmAPI)
			m_hDwmAPI = GetModuleHandle(_T("dwmapi.dll"));
		if(!m_hDwmAPI)
			m_hDwmAPI = LoadLibrary(_T("dwmapi.dll"));
		if(m_hDwmAPI)
		{
			BOOL bEnabled = FALSE;
			MARGINS Margins = { -1 };
			*(FARPROC *)&DwmIsCompositionEnabled      = GetProcAddress(m_hDwmAPI, "DwmIsCompositionEnabled");
			*(FARPROC *)&DwmExtendFrameIntoClientArea = GetProcAddress(m_hDwmAPI, "DwmExtendFrameIntoClientArea");
			*(FARPROC *)&DwmEnableBlurBehindWindow    = GetProcAddress(m_hDwmAPI, "DwmEnableBlurBehindWindow");
			if(SUCCEEDED(DwmIsCompositionEnabled(&bEnabled)) && bEnabled)
			{
				if(SUCCEEDED(DwmExtendFrameIntoClientArea(m_hWnd, &Margins)))
				{
					DWM_BLURBEHIND blurBehind ={DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED,TRUE, NULL, TRUE,};
					DwmEnableBlurBehindWindow(m_hWnd, &blurBehind);
				}
			}
		}
	}
	else
	{
		DWM_BLURBEHIND blurBehind = { 0 };
		blurBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED;
		blurBehind.fEnable = false;
		DwmEnableBlurBehindWindow(m_hWnd,&blurBehind);
	}
}

LRESULT IWindowBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLongPtr(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLongPtr(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	if(g_Shell == WinVista || g_Shell == Win7)
		pm.Init(m_hWnd,D2D_RENDER);
	else if(g_Shell == WinXP)
		pm.Init(m_hWnd,SKIA_RENDER);
	else
		pm.Init(m_hWnd,GDI_RENDER);
	pm.SetOwner(this);
	pm.AddPreMessageFilter(this);
	pm.SetResourcePath(GetSkinFolder().GetData());

	switch(GetResourceType())
	{
	case UILIB_ZIP:
		if(!ZipSkinPath.IsEmpty())
			pm.SetResourceZip(ZipSkinPath.GetData(),true);
		break;
	case UILIB_ZIPRESOURCE:
		{
			CDuiString szOldResID = CPaintManagerUI::GetResourceZip();
			int nID = (int)GetResourceID();
			CDuiString szCurResID;
			szCurResID.Format(_T("RESID:%d"),nID);
			if(szOldResID == szCurResID) break;
			HRSRC hResource = ::FindResource(pm.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
			if( hResource == NULL )
				return 0L;
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(pm.GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
				::FreeResource(hResource);
				return 0L;
			}
			dwSize = ::SizeofResource(pm.GetResourceDll(), hResource);
			if( dwSize == 0 )
				return 0L;
			m_lpResourceZIPBuffer = new BYTE[ dwSize ];
			if (m_lpResourceZIPBuffer != NULL)
				::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
			::FreeResource(hResource);
			char *pwd = GetResourcePwd();
			if(pwd)
				Encrypt::DecryptMem(pwd,&m_lpResourceZIPBuffer,dwSize);
			pm.SetResourceZip(szCurResID,m_lpResourceZIPBuffer, dwSize);
		}
		break;
	}


	ChooseLanuage();
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &pm);

	if(!pRoot){
		MessageBox(m_hWnd,_T("初始化界面资源失败！"),_T("异常信息"),MB_OK|MB_ICONERROR);
		this->Close(IDCLOSE);
	}
	pRoot->SetName(_T("Root"));
	pm.AttachDialog(pRoot);
	pm.AddNotifier(this);

#pragma region 窗口关闭按钮
	mpCloseBtn					= static_cast<CButtonUI*>(pm.FindControl(_T("SysCloseBtn")));
	mpMaxBtn					= static_cast<CButtonUI*>(pm.FindControl(_T("SysMaxBtn")));
	mpRestoreBtn				= static_cast<CButtonUI*>(pm.FindControl(_T("SysRestoreBtn")));
	mpMinBtn					= static_cast<CButtonUI*>(pm.FindControl(_T("SysMinBtn")));
#pragma endregion 窗口关闭按钮
	Init();
	if(pm.IsWndGlass())
		StartGlassEffect();
	return 0;

}

LRESULT IWindowBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(pm.GetEventSource()){
		TEventUI mEvent = {0};
		mEvent.Type			= uMsg;
		mEvent.lParam		= lParam;
		mEvent.wParam		= wParam;
		mEvent.ptMouse.x	= 0;
		mEvent.ptMouse.y	= 0;
		mEvent.wKeyState	= 0;
		mEvent.dwTimestamp	= GetTickCount();
		mEvent.pSender		= NULL;
		mEvent.chKey		= NULL;

		bHandled = !pm.GetEventSource()(&mEvent);

		return bHandled;
	}
	bHandled = FALSE;
	return 0;
}

void IWindowBase::Notify( TNotifyUI& msg )
{
	try
	{
		if( msg.sType == _T("click") )//鼠标点击消息
		{
			if( msg.pSender == mpCloseBtn )
			{
				Close(IDCLOSE);
			}
			else if( msg.pSender == mpMinBtn )
			{ 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);return;
			}
			else if( msg.pSender == mpMaxBtn)
			{
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);return;
			}
			else if( msg.pSender == mpRestoreBtn)
			{
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return;
			}
		}
	}
	catch(...)
	{
		throw "IWindowBase::Notify";
	}
}

LRESULT IWindowBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:         lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:          lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:        lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;

	case WM_NCACTIVATE:     lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:     lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:        lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:      lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:  lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;

	case WM_SIZE:           lRes = OnSize(uMsg, wParam, lParam, bHandled); break;  
	case WM_SYSCOMMAND:     lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:        lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:      lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:       lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:      lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:    lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_FIRSTLAYOUT:	StartUnLayeredShadow(); break;
	default:                bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled)
		return lRes;

	if (pm.MessageHandler(uMsg, wParam, lParam, lRes))
		return lRes;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT IWindowBase::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
{
	try
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent(wParam);
			default:
				break;
			}
		}
		else if(uMsg == WM_SYSKEYDOWN)
		{
			switch (wParam)
			{
			case VK_F4:
				{
					if(mpCloseBtn)
						pm.SendNotify(mpCloseBtn,_T("click"));
					return FALSE;
				}
			default:
				break;
			}
		}
		return FALSE;
	}
	catch(...)
	{
		throw "IWindowBase::MessageHandler";
	}
}

LRESULT IWindowBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		Close(IDCLOSE);
		return TRUE;
	}
	return FALSE;
}

CPaintManagerUI* IWindowBase::GetPaintManager()
{
	return &pm;
}

BOOL IWindowBase::IsInStaticControl(CControlUI *pControl)
{
	BOOL bRet = FALSE;
	if (! pControl)
	{
		return bRet;
	}

	CDuiString strClassName;
	std::vector<CDuiString> vctStaticName;

	strClassName = pControl->GetClass();
	strClassName.MakeLower();
	vctStaticName.push_back(_T("controlui"));
	vctStaticName.push_back(_T("textui"));
	vctStaticName.push_back(_T("labelui"));
	vctStaticName.push_back(_T("containerui"));
	vctStaticName.push_back(_T("horizontallayoutui"));
	vctStaticName.push_back(_T("verticallayoutui"));
	vctStaticName.push_back(_T("tablayoutui"));
	vctStaticName.push_back(_T("childlayoutui"));
	vctStaticName.push_back(_T("dialoglayoutui"));

	std::vector<CDuiString>::iterator it = std::find(vctStaticName.begin(), vctStaticName.end(), strClassName);
	if (vctStaticName.end() != it)
	{
		CControlUI* pParent = pControl->GetParent();
		while (pParent)
		{
			strClassName = pParent->GetClass();
			strClassName.MakeLower();
			it = std::find(vctStaticName.begin(), vctStaticName.end(), strClassName);
			if (vctStaticName.end() == it)
			{
				return bRet;
			}

			pParent = pParent->GetParent();
		}

		bRet = TRUE;
	}

	return bRet;
}

UINT IWindowBase::GetClassStyle() const
{
	return CS_DBLCLKS;
}

UILIB_RESOURCETYPE IWindowBase::GetResourceType() const
{
	return UILIB_FILE;
}

char* IWindowBase::GetResourcePwd() const
{
	return NULL;
}

LPCTSTR IWindowBase::GetResourceID() const
{
	return _T("");
}

void IWindowBase::StartUnLayeredShadow()
{
	if(!m_pWndShadow)
	{
		m_pWndShadow = new CWndShadow;
		m_pWndShadow->Create(m_hWnd);
		m_pWndShadow->Hide();
		m_pWndShadow->SetSize(5);
		m_pWndShadow->SetPosition(2,2);
		m_pWndShadow->SetSharpness(13);
		m_pWndShadow->SetDarkness(100);
		m_pWndShadow->SetColor(RGB(80, 80, 80));
		m_pWndShadow->Show(m_hWnd);
		m_pWndShadow->Update(m_hWnd);
	}
}
