#include "stdafx.h"
#include <zmouse.h>

namespace UiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
#define IDB_ICONS16 200
#define IDB_ICONS24 201
#define IDB_ICONS32 202
#define IDB_ICONS50 203
	//
	typedef struct tagFINDTABINFO
	{
		CControlUI* pFocus;
		CControlUI* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		TCHAR ch;
		bool bPickNext;
	} FINDSHORTCUT;

	typedef struct tagTIMERINFO
	{
		CControlUI* pSender;
		UINT nLocalID;
		HWND hWnd;
		UINT uWinTimer;
		bool bKilled;
	} TIMERINFO;

	/////////////////////////////////////////////////////////////////////////////////////
#ifdef UILIB_D3D
	CAnimationSpooler m_anim;
#endif
	HPEN m_hPens[UICOLOR__LAST] = { 0 };
	HFONT m_hFonts[UIFONT__LAST] = { 0 };
	HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
	LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
	COLORREF m_clrColors[UICOLOR__LAST][2] = { 0 };
	TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };
	HIMAGELIST m_himgIcons16 = NULL;
	HIMAGELIST m_himgIcons24 = NULL;
	HIMAGELIST m_himgIcons32 = NULL;
	HIMAGELIST m_himgIcons50 = NULL;
	HPEN m_hUpdateRectPen = NULL;
	HINSTANCE CPaintManagerUI::m_hInstance = NULL;
	HINSTANCE CPaintManagerUI::m_hResourceInstance = NULL;
	CDuiString CPaintManagerUI::m_pStrDefaultFontName = _T("宋体");
	CDuiString CPaintManagerUI::m_pStrResourcePath;
	CDuiString CPaintManagerUI::m_pStrResourceZip;
	bool CPaintManagerUI::m_bCachedResourceZip = false;
	HANDLE CPaintManagerUI::m_hResourceZip = NULL;
	short CPaintManagerUI::m_H = 180;
	short CPaintManagerUI::m_S = 100;
	short CPaintManagerUI::m_L = 100;
	CStdPtrArray CPaintManagerUI::m_aPreMessages;
	CStdPtrArray CPaintManagerUI::m_aPlugins;
	CWaitableHandleMgr CPaintManagerUI::m_WaitForHandlesMgr;

	CPaintManagerUI::CPaintManagerUI() :
		m_hWndPaint(NULL),
		m_hwndTooltip(NULL),
		m_bShowUpdateRect(false),
		m_uTimerID(0x1000),
		m_pRoot(NULL),
		m_pFocus(NULL),
		m_pDefaultFocus(NULL),
		m_pCapture(NULL),
		m_pEventEnter(NULL),
		m_pEventClick(NULL),
		m_pEventKey(NULL),
		m_bFirstLayout(true),
		m_bFocusNeeded(false),
		m_bUpdateNeeded(false),
		m_bMouseTracking(false),
		m_bMouseCapture(false),
		m_bUsedVirtualWnd(false),
		m_bUserGlass(false),
		m_bWndShadow(false),
		m_bUserMultiLanguage(false),
		m_pOwnerWnd(NULL),
		m_nOpacity(255),
		m_pParentResourcePM(NULL),
		m_pRenderCore(NULL)
	{
		m_dwDefaultDisabledColor		= 0xFFA7A6AA;
		m_dwDefaultFontColor			= 0xFF000000;
		m_dwDefaultLinkFontColor		= 0xFF0000FF;
		m_dwDefaultLinkHoverFontColor	= 0xFFD3215F;
		m_dwDefaultSelectedBkColor		= 0xFFBAE4FF;
		m_sCurStylesName.Empty();

		if( m_hUpdateRectPen == NULL ) {
			m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
			// Boot Windows Common Controls (for the ToolTip control)
			::InitCommonControls();
			::LoadLibrary(_T("msimg32.dll"));
		}

		m_pAnimateManager = NULL;

		m_szMinWindow.cx = 0;
		m_szMinWindow.cy = 0;
		m_szMaxWindow.cx = 0;
		m_szMaxWindow.cy = 0;
		m_szInitWindowSize.cx = 0;
		m_szInitWindowSize.cy = 0;
		m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
		::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
		::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
		m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
	}

	CPaintManagerUI::~CPaintManagerUI()
	{
		// Delete the control-tree structures
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
		m_mNameHash.Resize(0);
		if(m_pRoot)
		{
			delete m_pRoot;
			m_pRoot = NULL;
		}
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
#ifdef UILIB_D3D
		RemoveAllEffectStyle();
#endif
		RemoveAllOptionGroups();
		RemoveAllTimers();
		RemoveActionScriptGroupAll();
		RemoveAllStyles();
		RemoveAllWndContainer();
		if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
		m_aPreMessages.Remove(m_aPreMessages.Find(this));
		SAFE_DELETE(m_pRenderCore);
		SAFE_DELETE(m_pAnimateManager);
	}

	void CPaintManagerUI::Init(HWND hWnd,RenderWay _RenderWay)
	{
		ASSERT(::IsWindow(hWnd));
		m_hWndPaint = hWnd;
		m_RenderWay = SKIA_RENDER;
		m_pRenderCore = CUIRenderCore::GetInstance(m_RenderWay);
		m_pRenderCore->SetPaintWindow(hWnd);
		m_aPreMessages.Add(this);
		m_pAnimateManager = new CUIAnimateManager(this);
	}

	RenderWay CPaintManagerUI::GetRenderWay()
	{
		return m_RenderWay;
	}

	CWindowWnd* CPaintManagerUI::GetOwner()
	{
		return m_pOwnerWnd;
	}

	void CPaintManagerUI::SetOwner(LPVOID pOwner)
	{
		if(pOwner)
			m_pOwnerWnd = (CWindowWnd *)pOwner;
	}

	HINSTANCE CPaintManagerUI::GetInstance()
	{
		return m_hInstance;
	}

	CDuiString CPaintManagerUI::GetInstancePath()
	{
		if( m_hInstance == NULL ) return _T('\0');

		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
		CDuiString sInstancePath = tszModule;
		int pos = sInstancePath.ReverseFind(_T('\\'));
		if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
		return sInstancePath;
	}

	CDuiString CPaintManagerUI::GetCurrentPath()
	{
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetCurrentDirectory(MAX_PATH, tszModule);
		return tszModule;
	}

	HINSTANCE CPaintManagerUI::GetResourceDll()
	{
		if( m_hResourceInstance == NULL ) return m_hInstance;
		return m_hResourceInstance;
	}

	const CDuiString& CPaintManagerUI::GetResourcePath()
	{
		return m_pStrResourcePath;
	}

	const CDuiString& CPaintManagerUI::GetResourceZip()
	{
		return m_pStrResourceZip;
	}

	bool CPaintManagerUI::IsCachedResourceZip()
	{
		return m_bCachedResourceZip;
	}

	HANDLE CPaintManagerUI::GetResourceZipHandle()
	{
		return m_hResourceZip;
	}

	void CPaintManagerUI::SetInstance(HINSTANCE hInst)
	{
		m_hInstance = hInst;
		CWndShadow::Initialize(hInst);

		REGIST_DUICLASS_STATIC(CLockerItemUI);
		REGIST_DUICLASS_STATIC(CLockerUI);
		REGIST_DUICLASS_STATIC(CMaskControlUI);
		REGIST_DUICLASS_STATIC(CActiveXCtrl);
		REGIST_DUICLASS_STATIC(CWebBrowserUI);
		REGIST_DUICLASS_STATIC(CButtonUI);
		REGIST_DUICLASS_STATIC(CCalendarUI);
		REGIST_DUICLASS_STATIC(CChartViewUI);
		REGIST_DUICLASS_STATIC(CCheckBoxUI);
		REGIST_DUICLASS_STATIC(CComboUI);
		REGIST_DUICLASS_STATIC(CComboBoxUI);
		REGIST_DUICLASS_STATIC(CDateTimeUI);
		REGIST_DUICLASS_STATIC(CEditUI);
		REGIST_DUICLASS_STATIC(CFadeButtonUI);
		REGIST_DUICLASS_STATIC(CFlashUI);
		REGIST_DUICLASS_STATIC(CFlashWndUI);
		REGIST_DUICLASS_STATIC(CGifAnimUI);
		REGIST_DUICLASS_STATIC(CLabelUI);
		REGIST_DUICLASS_STATIC(CListUI);
		REGIST_DUICLASS_STATIC(CListHeaderUI);
		REGIST_DUICLASS_STATIC(CListHeaderItemUI);
		REGIST_DUICLASS_STATIC(CListLabelElementUI);
		REGIST_DUICLASS_STATIC(CListTextElementUI);
		REGIST_DUICLASS_STATIC(CListImageTextElementUI);
		REGIST_DUICLASS_STATIC(CListContainerElementUI);
		REGIST_DUICLASS_STATIC(CListContainerItemElementUI);
		REGIST_DUICLASS_STATIC(CMenuElementUI);
		REGIST_DUICLASS_STATIC(CMenuUI);
		REGIST_DUICLASS_STATIC(COptionUI);
		REGIST_DUICLASS_STATIC(CProgressUI);
		REGIST_DUICLASS_STATIC(CRadioBoxUI);
		REGIST_DUICLASS_STATIC(CRichEditUI);
		REGIST_DUICLASS_STATIC(CScrollBarUI);
		REGIST_DUICLASS_STATIC(CSliderUI);
		REGIST_DUICLASS_STATIC(CTabItemUI);
		REGIST_DUICLASS_STATIC(CTabBodyUI);
		REGIST_DUICLASS_STATIC(CTabUI);
		REGIST_DUICLASS_STATIC(CTextUI);
		REGIST_DUICLASS_STATIC(CToolTipsUI);
		REGIST_DUICLASS_STATIC(CTreeNodeUI);
		REGIST_DUICLASS_STATIC(CTreeViewUI);
		REGIST_DUICLASS_STATIC(CWndContainerUI);
		REGIST_DUICLASS_STATIC(CContainerUI);
		REGIST_DUICLASS_STATIC(CControlUI);
		REGIST_DUICLASS_STATIC(CAnimationTabLayoutUI);
		REGIST_DUICLASS_STATIC(CChildLayoutUI);
		REGIST_DUICLASS_STATIC(CHorizontalLayoutUI);
		REGIST_DUICLASS_STATIC(CTabLayoutUI);
		REGIST_DUICLASS_STATIC(CTileLayoutUI);
		REGIST_DUICLASS_STATIC(CVerticalLayoutUI);

	}

	void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
	{
		::SetCurrentDirectory(pStrPath);
	}

	void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
	{
		m_hResourceInstance = hInst;
	}

	void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
	{
		m_pStrResourcePath = pStrPath;
		if( m_pStrResourcePath.IsEmpty() ) return;
		TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
		if( cEnd != _T('\\') && cEnd != _T('/') ) m_pStrResourcePath += _T('\\');
	}

	void CPaintManagerUI::SetResourceZip(LPCTSTR resID,LPVOID pVoid, unsigned int len)
	{
		if( !m_pStrResourceZip.IsEmpty() && m_pStrResourceZip == resID ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = resID;
		m_bCachedResourceZip = true;
		m_hResourceZip = (HANDLE)OpenZip(pVoid, len, 3);
	}

	void CPaintManagerUI::SetResourceZip(LPCTSTR pStrPath, bool bCachedResourceZip)
	{
		if( m_pStrResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = pStrPath;
		m_bCachedResourceZip = bCachedResourceZip;
		if( m_bCachedResourceZip ) {
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			sFile += CPaintManagerUI::GetResourceZip();
			m_hResourceZip = (HANDLE)OpenZip((void*)sFile.GetData(), 0, 2);
		}
	}

	void CPaintManagerUI::GetHSL(short* H, short* S, short* L)
	{
		*H = m_H;
		*S = m_S;
		*L = m_L;
	}

	void CPaintManagerUI::SetHSL(bool bUseHSL, short H, short S, short L)
	{
		if( H == m_H && S == m_S && L == m_L ) return;
		m_H = CLAMP(H, 0, 360);
		m_S = CLAMP(S, 0, 200);
		m_L = CLAMP(L, 0, 200);
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			if( pManager != NULL && pManager->GetRoot() != NULL )
				pManager->GetRoot()->Invalidate();
		}
	}

	void CPaintManagerUI::ReloadSkin()
	{
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			pManager->ReloadAllImages();
		}
	}

	bool CPaintManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
	{
		ASSERT( !::IsBadStringPtr(pstrModuleName,-1) || pstrModuleName == NULL );
		if( pstrModuleName == NULL ) return false;
		HMODULE hModule = ::LoadLibrary(pstrModuleName);
		if( hModule != NULL ) {
			LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
			if( lpCreateControl != NULL ) {
				if( m_aPlugins.Find(lpCreateControl) >= 0 ) return true;
				m_aPlugins.Add(lpCreateControl);
				return true;
			}
		}
		return false;
	}

	CStdPtrArray* CPaintManagerUI::GetPlugins()
	{
		return &m_aPlugins;
	}

	HWND CPaintManagerUI::GetPaintWindow() const
	{
		return m_hWndPaint;
	}

	HWND CPaintManagerUI::GetTooltipWindow() const
	{
		return m_hwndTooltip;
	}

	HDC CPaintManagerUI::GetPaintDC() const
	{
		HDC hDC = m_pRenderCore->GetRenderHDC(FALSE);
		return hDC;
	}

	POINT CPaintManagerUI::GetMousePos() const
	{
		return m_ptLastMousePos;
	}

	SIZE CPaintManagerUI::GetClientSize() const
	{
		RECT rcClient = { 0 };
		::GetClientRect(m_hWndPaint, &rcClient);
		return CDuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}

	SIZE CPaintManagerUI::GetInitSize()
	{
		return m_szInitWindowSize;
	}

	void CPaintManagerUI::SetInitSize(int cx, int cy)
	{
		m_szInitWindowSize.cx = cx;
		m_szInitWindowSize.cy = cy;
		if( m_pRoot == NULL && m_hWndPaint != NULL ) {
			::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	RECT& CPaintManagerUI::GetSizeBox()
	{
		return m_rcSizeBox;
	}

	void CPaintManagerUI::SetSizeBox(RECT& rcSizeBox)
	{
		m_rcSizeBox = rcSizeBox;
	}

	RECT& CPaintManagerUI::GetCaptionRect()
	{
		return m_rcCaption;
	}

	void CPaintManagerUI::SetCaptionRect(RECT& rcCaption)
	{
		m_rcCaption = rcCaption;
	}

	SIZE CPaintManagerUI::GetRoundCorner() const
	{
		return m_szRoundCorner;
	}

	void CPaintManagerUI::SetRoundCorner(int cx, int cy)
	{
		m_szRoundCorner.cx = cx;
		m_szRoundCorner.cy = cy;
	}

	SIZE CPaintManagerUI::GetMinInfo() const
	{
		return m_szMinWindow;
	}

	void CPaintManagerUI::SetMinInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMinWindow.cx = cx;
		m_szMinWindow.cy = cy;
	}

	SIZE CPaintManagerUI::GetMaxInfo() const
	{
		return m_szMaxWindow;
	}

	void CPaintManagerUI::SetMaxInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMaxWindow.cx = cx;
		m_szMaxWindow.cy = cy;
	}

	int CPaintManagerUI::GetTransparent() const
	{
		return m_nOpacity;
	}
	void CPaintManagerUI::SetTransparent(int nOpacity)
	{
		if (nOpacity<0)
			m_nOpacity = 0;
		else if (nOpacity>255)
			m_nOpacity = 255;
		else
			m_nOpacity = nOpacity;
		if( m_hWndPaint != NULL ) {
			typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
			PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32)
			{
				fSetLayeredWindowAttributes = 
					(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				if( fSetLayeredWindowAttributes == NULL ) return;
			}

			DWORD dwStyle = ::GetWindowLongPtr(m_hWndPaint, GWL_EXSTYLE);
			DWORD dwNewStyle = dwStyle;
			if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
			else dwNewStyle &= ~WS_EX_LAYERED;
			if(dwStyle != dwNewStyle) ::SetWindowLongPtr(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
			fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
		}
	}

	void CPaintManagerUI::SetBackgroundTransparent(bool bTrans)
	{
		if(m_pRenderCore)
			m_pRenderCore->SetWindowLayered(bTrans);
	}

	void CPaintManagerUI::SetUnLayerdWndShadow(bool bShadow)
	{
		m_bWndShadow = bShadow;
	}

	bool CPaintManagerUI::IsWndShadow()
	{
		return m_bWndShadow;
	}

	void CPaintManagerUI::SetWndGlassEffect(bool bGlass)
	{
		m_bUserGlass = bGlass;
		if(m_pRenderCore)
			m_pRenderCore->SetWindowGlass(m_bUserGlass);
	}

	bool CPaintManagerUI::IsShowUpdateRect() const
	{
		return m_bShowUpdateRect;
	}

	void CPaintManagerUI::SetShowUpdateRect(bool show)
	{
		m_bShowUpdateRect = show;
	}

	bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				return true;
			}
		}
		switch( uMsg ) {
	case WM_KEYDOWN:
		{
			// Tabbing between controls
			if( wParam == VK_TAB ) {
				if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL ) {
					if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return false;
				}
				SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
				return true;
			}
		}
		break;
	case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			FINDSHORTCUT fs = { 0 };
			fs.ch = toupper((int)wParam);
			CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
			if( pControl != NULL ) {
				pControl->SetFocus();
				pControl->Activate();
				return true;
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
			if( m_pFocus != NULL ) {
				TEventUI event = { 0 };
				event.Type = UIEVENT_SYSKEY;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
		}
		break;
		}
		return false;
	}

	bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		//#ifdef _DEBUG
		//    switch( uMsg ) {
		//    case WM_NCPAINT:
		//    case WM_NCHITTEST:
		//    case WM_SETCURSOR:
		//       break;
		//    default:
		//       DUITRACE(_T("MSG: %-20s (%08ld)"), TRACEMSG(uMsg), ::GetTickCount());
		//    }
		//#endif
		// Not ready yet?
		if( m_hWndPaint == NULL ) return false;

		TNotifyUI* pMsg = NULL;
		while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
			m_aAsyncNotify.Remove(0);
			if( pMsg->pSender != NULL ) {
				if( pMsg->pSender->OnNotify ){
					if(pMsg->pSender->OnNotify(pMsg)){
						delete pMsg;
						return true;
					}
				}
			}
			for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) 
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			delete pMsg;
		}

		// Cycle through listeners
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				lRes = lResult;
				return true;
			}
		}
		// Custom handling of events
		switch( uMsg ) {
	case WM_APP + 1:
		{
			for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
				delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
			m_aDelayedCleanup.Empty();
		}
		break;
	case WM_USER + 2:
		{
			if(m_pFocus == NULL)
				SetFocus(m_pDefaultFocus);
			else
			{
				if(m_pFocus->GetInterface(_T("Edit")) != NULL || m_pFocus->GetInterface(_T("ComboBox")) != NULL)
				{
					TEventUI event = { 0 };
					event.Type = UIEVENT_SETFOCUS;
					event.pSender = m_pFocus;
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
					SendNotify(m_pFocus, _T("setfocus"));
				}
			}
		}
		break;
	case WM_CLOSE:
		{
			// Make sure all matching "closing" events are sent
			TEventUI event = { 0 };
			event.ptMouse = m_ptLastMousePos;
			event.dwTimestamp = ::GetTickCount();
			if( m_pEventEnter != NULL ) {
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventEnter;
				m_pEventEnter->Event(event);
			}
			if( m_pEventClick != NULL ) {
				event.Type = UIEVENT_BUTTONUP;
				event.pSender = m_pEventClick;
				m_pEventClick->Event(event);
			}

			SetFocus(NULL);

			// Hmmph, the usual Windows tricks to avoid
			// focus loss...
			HWND hwndParent = GetWindowOwner(m_hWndPaint);
			if( hwndParent != NULL ) ::SetFocus(hwndParent);
		}
		break;
	case WM_ERASEBKGND:
		{
			// We'll do the painting here...
			lRes = 1;
			return true;
		}
#ifdef UILIB_D3D
	case WM_EFFECTS:
		{
			if( m_anim.IsAnimating() )
			{
				// 3D animation in progress
				m_anim.Render();
				// Do a minimum paint loop
				// Keep the client area invalid so we generate lots of
				// 3 messages. Cross fingers that Windows doesn't
				// batch these somehow in the future.
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				::EndPaint(m_hWndPaint, &ps);
				::InvalidateRect(m_hWndPaint, NULL, FALSE);
			}
			else if( m_anim.IsJobScheduled() ) {
				// Animation system needs to be initialized
				m_anim.Init(m_hWndPaint);
				// A 3D animation was scheduled; allow the render engine to
				// capture the window content and repaint some other time
				if( !m_anim.PrepareAnimation(m_hWndPaint)) 
					m_anim.CancelJobs();
				::InvalidateRect(m_hWndPaint, NULL, TRUE);
			}
		}
		return true;
#endif
	case WM_PAINT:
		{
			RECT rcPaint = { 0 };
			if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) 
				return true;
			if( m_pRoot == NULL ) 
			{
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				::EndPaint(m_hWndPaint, &ps);
				return true;
			}
			if( m_bUpdateNeeded ) 
			{
				m_bUpdateNeeded = false;
				RECT rcClient = { 0 };
				::GetClientRect(m_hWndPaint, &rcClient);
				if( !::IsRectEmpty(&rcClient) )
				{
					if( m_pRoot->IsUpdateNeeded() )
					{
						m_pRoot->SetPos(rcClient);
						m_pRenderCore->ReleaseRenderObj();
					}
					else 
					{
						CControlUI* pControl = NULL;
						while( pControl = m_pRoot->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) )
							pControl->SetPos( pControl->GetPos() );
					}
					if( m_bFirstLayout ) 
					{
						m_bFirstLayout = false;
						if(IsWndShadow() && m_pOwnerWnd)
							m_pOwnerWnd->SendMessage(WM_FIRSTLAYOUT);
						SendNotify(m_pRoot, _T("windowinit"),  0, 0, false);
					}
				}
			}
			if(m_bFocusNeeded)
			{
				if(m_pDefaultFocus)
				{
					SetFocus(m_pDefaultFocus);
					m_bFocusNeeded = false;
				}
				else
					SetNextTabControl();
			}

#ifdef UILIB_D3D
			if(m_anim.IsAnimating() || m_anim.IsJobScheduled())
				::PostMessage(m_hWndPaint,WM_EFFECTS,NULL,NULL);
			else
			{
#endif
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				m_pRenderCore->BeginDraw(&rcPaint);
				m_pRoot->DoPaint(rcPaint);
				for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) 
				{
					CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
					pPostPaintControl->DoPostPaint(rcPaint);
				}
				m_pRenderCore->EndDraw(ps);
				if( m_bShowUpdateRect && m_pRenderCore->GetRenderType() != D2D_RENDER)
				{
					HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
					::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
					::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
					::SelectObject(ps.hdc, hOldPen);
				}
				::EndPaint(m_hWndPaint, &ps);
#ifdef UILIB_D3D
			}
#endif
		}
		//绘制过程中的Resize导致重绘;
		if( m_bUpdateNeeded )
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
		return true;
	case WM_PRINTCLIENT:
		{
			RECT rcClient;
			::GetClientRect(m_hWndPaint, &rcClient);
			TRenderObj* pRenderObj = (TRenderObj *)wParam;
			HDC hDC = pRenderObj->hDC ;
			int save = ::SaveDC(hDC);

			m_pRenderCore->SetRenderObj(pRenderObj);
			m_pRoot->DoPaint(rcClient);
			m_pRenderCore->ReleaseRenderObj();

			if(m_pRenderCore->IsWindowLayered())
			{
				DWORD dwExStyle=GetWindowLongPtr(m_hWndPaint,GWL_EXSTYLE);
				dwExStyle &= ~WS_EX_LAYERED;
				SetWindowLongPtr(m_hWndPaint,GWL_EXSTYLE,dwExStyle);
				::BitBlt(GetDC(m_hWndPaint),0,0,rcClient.right-rcClient.left,rcClient.bottom-rcClient.top,hDC,0,0,SRCCOPY);
			}
			// Check for traversing children. The crux is that WM_PRINT will assume
			// that the DC is positioned at frame coordinates and will paint the child
			// control at the wrong position. We'll simulate the entire thing instead.
			if( (lParam & PRF_CHILDREN) != 0 ) {
				HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
				while( hWndChild != NULL ) {
					RECT rcPos = { 0 };
					::GetWindowRect(hWndChild, &rcPos);
					::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
					::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
					// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
					//       since the latter will not print the nonclient correctly for
					//       EDIT controls.
					::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
					hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
				}
			}
			
			::RestoreDC(hDC, save);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
			if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
			if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
			if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
			if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
		}
		break;
	case WM_MOVE:
		{
			int nCount = m_aWndContainers.GetSize();
			if(nCount)
			{
				TEventUI event = { 0 };
				event.Type = UIEVENT_WMMOVE;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
				for(int i = 0 ;i < nCount ; i++)
				{
					CControlUI *pEventActer = static_cast<CControlUI *>(m_aWndContainers[i]);
					event.pSender = pEventActer;
					pEventActer->Event(event);
				}
			}

		}
		break;
	case WM_SIZE:
		{
			if(m_pRenderCore)
				m_pRenderCore->Resize();
			int nCount = m_aWndContainers.GetSize();
			if(nCount)
			{
				TEventUI event = { 0 };
				event.Type = UIEVENT_WINDOWSIZE;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
				for(int i = 0 ;i < nCount ; i++)
				{
					CControlUI *pEventActer = static_cast<CControlUI *>(m_aWndContainers[i]);
					event.pSender = pEventActer;
					pEventActer->Event(event);
				}
			}
			if( m_pFocus != NULL ) {
				TEventUI event = { 0 };
				event.Type = UIEVENT_WINDOWSIZE;
				event.pSender = m_pFocus;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
#ifdef UILIB_D3D
			//当对话框尺寸变化时 删除动画 job
			if( m_anim.IsAnimating() )
				m_anim.CancelJobs();
#endif
			m_bUpdateNeeded = true;
			if(m_pRenderCore && m_pRenderCore->IsWindowLayered())
				InvalidateRect(m_hWndPaint,NULL,FALSE);
			if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
		}
		return true;
	case WM_TIMER:
		{
			for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
				const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
				if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_TIMER;
					event.pSender = pTimer->pSender;
					event.wParam = pTimer->nLocalID;
					event.dwTimestamp = ::GetTickCount();
					pTimer->pSender->Event(event);
					break;
				}
			}
		}
		break;
	case WM_MOUSEHOVER:
		{
			m_bMouseTracking = true;
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			CControlUI* pHover = FindControl(pt);
			if( pHover == NULL ) break;
			// Generate mouse hover event
			if( pHover != NULL ) {
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.Type = UIEVENT_MOUSEHOVER;
				event.pSender = pHover;
				event.dwTimestamp = ::GetTickCount();
				pHover->Event(event);
			}
			// Create tooltip information
			CDuiString sToolTip = pHover->GetToolTip();
			if( sToolTip.IsEmpty() ) return true;
			::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND;
			m_ToolTip.hwnd = m_hWndPaint;
			m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
			m_ToolTip.hinst = m_hInstance;
			m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
			m_ToolTip.rect = pHover->GetPos();
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(0, _T("Tooltips_class32"), NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
				::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
			}
			::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
			::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
			::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
		}
		return true;
	case WM_MOUSELEAVE:
		{
			if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			if(m_pEventEnter)
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				TEventUI event = { 0 };
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventEnter;
				m_pEventEnter->Event(event);
				m_pEventEnter = NULL;
			}
			if(m_pEventClick)
				m_pEventClick = NULL;
			if(!m_pCapture) m_bMouseCapture = false;
			m_bMouseTracking = false;
		}
		break;
	case WM_NCMOUSEHOVER:
		{
			m_bMouseTracking = true;
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(m_hWndPaint,&pt);
			CControlUI* pHover = FindControl(pt);
			if( pHover == NULL ) break;
			// Generate mouse hover event
			if( pHover != NULL ) {
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.Type = UIEVENT_MOUSEHOVER;
				event.pSender = pHover;
				event.dwTimestamp = ::GetTickCount();
				pHover->Event(event);
			}
			// Create tooltip information
			CDuiString sToolTip = pHover->GetToolTip();
			if( sToolTip.IsEmpty() ) return true;
			::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND;
			m_ToolTip.hwnd = m_hWndPaint;
			m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
			m_ToolTip.hinst = m_hInstance;
			m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
			m_ToolTip.rect = pHover->GetPos();
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(0, _T("Tooltips_class32"), NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
				::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
			}
			::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
			::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
			::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
		}
		break;
	case WM_NCMOUSEMOVE:
		{
			if( !m_bMouseTracking ) 
			{
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_HOVER | TME_LEAVE | TME_NONCLIENT;
				tme.hwndTrack = m_hWndPaint;
				tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				_TrackMouseEvent(&tme);
				m_bMouseTracking = true;
			}
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(m_hWndPaint,&pt);
			if(pt.x == m_ptLastMousePos.x && pt.y == m_ptLastMousePos.y) break;
			TEventUI event = { 0 };
			event.ptMouse = m_ptLastMousePos = pt;
			event.dwTimestamp = ::GetTickCount();
			if(m_pCapture)
			{
				event.Type = UIEVENT_MOUSEMOVE;
				event.pSender = m_pCapture;
				m_pCapture->Event(event);
			}
			else
			{
				CControlUI* pCurPtControl = FindControl(pt);
				if( pCurPtControl != NULL && pCurPtControl->GetManager() != this ) break;
				if(m_pEventEnter && pCurPtControl != m_pEventEnter) 
				{
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventEnter;
					m_pEventEnter->Event(event);
					m_pEventEnter = NULL;
					if( m_hwndTooltip != NULL )
						::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
					m_bMouseTracking = false;
				}
				if(pCurPtControl && pCurPtControl != m_pEventEnter)
				{
					event.bHandle = false;
					event.Type = UIEVENT_MOUSEENTER;
					event.pSender = pCurPtControl;
					pCurPtControl->Event(event);
					m_pEventEnter = pCurPtControl;
				}
				if(pCurPtControl && m_pEventEnter == pCurPtControl)
				{
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pCurPtControl;
					pCurPtControl->Event(event);
				}
			}
		}
		break;
	case WM_NCMOUSELEAVE:
		{
			POINT pt = {0};
			GetCursorPos(&pt);
			RECT rcWnd = {0};
			GetWindowRect(m_hWndPaint,&rcWnd);
			if(!PtInRect(&rcWnd,pt))
			{
				if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				if(m_pEventEnter)
				{
					POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					TEventUI event = { 0 };
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventEnter;
					m_pEventEnter->Event(event);
					m_pEventEnter = NULL;
				}
				if(m_pEventClick)
					m_pEventClick = NULL;
				if(!m_pCapture) m_bMouseCapture = false;
				m_bMouseTracking = false;
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			// Start tracking this entire window again...
			if( !m_bMouseTracking ) {
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_HOVER | TME_LEAVE;
				tme.hwndTrack = m_hWndPaint;
				tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				_TrackMouseEvent(&tme);
				m_bMouseTracking = true;
			}
			// Generate the appropriate mouse messages
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if(pt.x == m_ptLastMousePos.x && pt.y == m_ptLastMousePos.y) break;
			TEventUI event = { 0 };
			event.ptMouse = m_ptLastMousePos = pt;
			event.dwTimestamp = ::GetTickCount();
			if(m_pCapture)
			{
				event.Type = UIEVENT_MOUSEMOVE;
				event.pSender = m_pCapture;
				m_pCapture->Event(event);
			}
			else
			{
				CControlUI* pCurPtControl = FindControl(pt);
				if( pCurPtControl != NULL && pCurPtControl->GetManager() != this ) break;
				if(m_pEventEnter && pCurPtControl != m_pEventEnter) 
				{
  					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventEnter;
					m_pEventEnter->Event(event);
					m_pEventEnter = NULL;
					if( m_hwndTooltip != NULL )
						::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
					m_bMouseTracking = false;
				}
				if(pCurPtControl && pCurPtControl != m_pEventEnter)
				{
					event.bHandle = false;
					event.Type = UIEVENT_MOUSEENTER;
					event.pSender = pCurPtControl;
					pCurPtControl->Event(event);
					m_pEventEnter = pCurPtControl;
				}
				if(pCurPtControl && m_pEventEnter == pCurPtControl)
				{
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pCurPtControl;
					pCurPtControl->Event(event);
				}
			}
		}
		break;
	case WM_NCLBUTTONDOWN:
		{
			if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			SetFocus(NULL);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			// We alway set focus back to our app (this helps
			// when Win32 child windows are placed on the dialog
			// and we need to remove them on focus change).
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( pControl->GetManager() != this ) break;
			m_pEventClick = pControl;
			pControl->SetFocus();
			SetCapture(pControl);
			TEventUI event = { 0 };
			event.Type = UIEVENT_BUTTONDOWN;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
#ifdef UILIB_D3D
			// No need to burden user with 3D animations
			m_anim.CancelJobs();
#endif
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			::SetFocus(m_hWndPaint);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( pControl->GetManager() != this ) break;
			SetCapture(pControl);
			TEventUI event = { 0 };
			event.Type = UIEVENT_DBLCLICK;
			event.pSender = pControl;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			ReleaseCapture();
			if( m_pEventClick == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_BUTTONUP;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			::SetFocus(m_hWndPaint);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( pControl->GetManager() != this ) break;
			pControl->SetFocus();
			SetCapture(pControl);
			TEventUI event = { 0 };
			event.Type = UIEVENT_RBUTTONDOWN;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_RBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL ) break;
			ReleaseCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_RBUTTONUP;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
		}
		break;
	case WM_CONTEXTMENU:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWndPaint, &pt);
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL ) break;
			ReleaseCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_CONTEXTMENU;
			event.pSender = m_pEventClick;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.lParam = (LPARAM)m_pEventClick;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_RELOADSTYLE:
		{
			if(m_pRoot)
			{
				TEventUI event		= { 0 };
				event.Type			= UIEVENT_RELOADSTYLE;
				event.pSender		= m_pRoot;
				event.wParam		= wParam;
				event.lParam		= lParam;
				event.ptMouse.x		= 0;
				event.ptMouse.y		= 0;
				event.wKeyState		= 0;
				event.dwTimestamp	= ::GetTickCount();
				EventAllControl(event,m_pRoot);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWndPaint, &pt);
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( pControl->GetManager() != this ) break;
			int zDelta = (int) (short) HIWORD(wParam);
			TEventUI event = { 0 };
			event.Type = UIEVENT_SCROLLWHEEL;
			event.pSender = pControl;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, zDelta);
			event.lParam = lParam;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);

			// Let's make sure that the scroll item below the cursor is the same as before...
			::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
		}
		break;
	case WM_CHAR:
		{
			if( m_pFocus == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_CHAR;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
		}
		break;
	case WM_KEYDOWN:
		{
			if( m_pFocus == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			m_pEventKey = m_pFocus;
		}
		break;
	case WM_KEYUP:
		{
			if( m_pEventKey == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYUP;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pEventKey->Event(event);
			m_pEventKey = NULL;
		}
		break;
	case WM_SETCURSOR:
		{
			if( LOWORD(lParam) != HTCLIENT ) break;
			if( m_bMouseCapture ) return true;

			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_hWndPaint, &pt);
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETCURSOR;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
		}
		return true;
	case WM_NOTIFY:
		{
			LPNMHDR lpNMHDR = (LPNMHDR) lParam;
			if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	case WM_COMMAND:
		{
			if( lParam == 0 ) break;
			HWND hWndChild = (HWND) lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
		{
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
			if( lParam == 0 ) break;
			HWND hWndChild = (HWND) lParam;
			if(IsWindow(hWndChild))
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	default:
		break;
		}

		pMsg = NULL;
		while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
			m_aAsyncNotify.Remove(0);
			if( pMsg->pSender != NULL ) {
				if( pMsg->pSender->OnNotify ){
					if(pMsg->pSender->OnNotify(pMsg)){
						delete pMsg;
						return true;
					}
				}
			}
			for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) 
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			delete pMsg;
		}

		return false;
	}

	void CPaintManagerUI::NeedUpdate()
	{
		m_bUpdateNeeded = true;
	}

	void CPaintManagerUI::Invalidate(RECT& rcItem)
	{
		::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
	}

	bool CPaintManagerUI::AttachDialog(CControlUI* pControl)
	{
		ASSERT(::IsWindow(m_hWndPaint));
		SetFocus(NULL);
		m_pEventKey = NULL;
		m_pEventEnter = NULL;
		m_pEventClick = NULL;
		// Remove the existing control-tree. We might have gotten inside this function as
		// a result of an event fired or similar, so we cannot just delete the objects and
		// pull the internal memory of the calling code. We'll delay the cleanup.
		if( m_pRoot != NULL ) {
			m_aPostPaintControls.Empty();
			AddDelayedCleanup(m_pRoot);
		}
		m_pRoot = pControl;
		m_bUpdateNeeded = true;
		m_bFirstLayout = true;
		m_bFocusNeeded = true;
		return InitControls(pControl);
	}

	bool CPaintManagerUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
	{
		ASSERT(pControl);
		if( pControl == NULL ) return false;
		pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
		pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
		return true;
	}

	void CPaintManagerUI::ReapObjects(CControlUI* pControl)
	{
		if( pControl == m_pEventKey ) m_pEventKey = NULL;
		if( pControl == m_pEventEnter ) m_pEventEnter = NULL;
		if( pControl == m_pEventClick ) m_pEventClick = NULL;
		if( pControl == m_pFocus ) m_pFocus = NULL;
		if( pControl == m_pCapture ) m_pCapture = NULL;
		KillTimer(pControl);
		const CDuiString& sName = pControl->GetName();
		if( !sName.IsEmpty() ) {
			if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
		}
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
			TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
			if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
		}    
	}

	bool CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					return false;
				}
			}
			aOptionGroup->Add(pControl);
		}
		else {
			CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
			aOptionGroup->Add(pControl);
			m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
		}
		return true;
	}

	CStdPtrArray* CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) return static_cast<CStdPtrArray*>(lp);
		return NULL;
	}

	void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			if( aOptionGroup == NULL ) return;
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					aOptionGroup->Remove(i);
					break;
				}
			}
			if( aOptionGroup->IsEmpty() ) {
				delete aOptionGroup;
				m_mOptionGroup.Remove(pStrGroupName);
			}
		}
	}

	void CPaintManagerUI::RemoveAllOptionGroups()
	{
		CStdPtrArray* aOptionGroup;
		for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
			if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
				aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
				delete aOptionGroup;
			}
		}
		m_mOptionGroup.RemoveAll();
	}

#ifdef UILIB_D3D
	bool CPaintManagerUI::AddEffectsStyle( LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue )
	{
		try
		{
			CDuiString* pEffectsStyleValue = new CDuiString(pStrStyleValue);

			if(pEffectsStyleValue != NULL)
			{
				if( m_mEffectsStyle.Find(pStrStyleName) == NULL )
					m_mEffectsStyle.Set(pStrStyleName,pEffectsStyleValue);
				else
					delete pEffectsStyleValue;
				pEffectsStyleValue = NULL;
				return true;
			}
			return false;
		}
		catch (...)
		{
			throw "CPaintManagerUI::AddEffectsStyle";
		}
	}

	UiLib::CDuiString CPaintManagerUI::GetEffectsStyle( LPCTSTR pStrStyleName )
	{
		try
		{
			CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
			if( !pEffectsStyleValue && m_pParentResourcePM ) return m_pParentResourcePM->GetEffectsStyle(pStrStyleName);

			if( pEffectsStyleValue ) return pEffectsStyleValue->GetData();
			else return CDuiString(_T(""));
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetEffectsStyle";
		}
	}

	const CStdStringPtrMap& CPaintManagerUI::GetEffectsStyles() const
	{
		try
		{
			return m_mEffectsStyle;
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetEffectsStyles";
		}
	}

	bool CPaintManagerUI::RemoveEffectStyle( LPCTSTR pStrStyleName )
	{
		try
		{
			CDuiString* pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(pStrStyleName));
			if( !pEffectsStyleValue ) return false;

			delete pEffectsStyleValue;
			return m_mEffectsStyle.Remove(pStrStyleName);
		}
		catch (...)
		{
			throw "CPaintManagerUI::RemoveEffectStyle";
		}
	}

	void CPaintManagerUI::RemoveAllEffectStyle()
	{
		try
		{
			CDuiString* pEffectsStyleValue;
			for( int i = 0; i< m_mEffectsStyle.GetSize(); i++ ) {
				if(LPCTSTR key = m_mEffectsStyle.GetAt(i)) {
					pEffectsStyleValue = static_cast<CDuiString*>(m_mEffectsStyle.Find(key));
					if(pEffectsStyleValue)
						delete pEffectsStyleValue;
					pEffectsStyleValue = NULL;
				}
			}
			m_mEffectsStyle.RemoveAll();
		}
		catch (...)
		{
			throw "CPaintManagerUI::RemoveAllEffectStyle";
		}
	}
#endif

	bool CPaintManagerUI::AddControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlsStyle = NULL;
		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle)
		{
			pControlsStyle = new CStdStringPtrMap();
			m_mStyles.Insert(pStylesName,(LPVOID)pControlsStyle);
		}

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));

		if(!pPtrMap){
			pPtrMap = new CStdStringPtrMap();
			pControlsStyle->Insert(pStrStyleName,(LPVOID)pPtrMap);
		}

		if(pPtrMap)
		{
			CDuiString* nVal = new CDuiString(pStrVal);

			if( pPtrMap->Find(pStrKey) == NULL )
				pPtrMap->Insert(pStrKey,nVal);
			else
				delete nVal;
			nVal = NULL;
			return true;
		}
		return false;
	}

	bool CPaintManagerUI::AddControlStyle( LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlsStyle;
		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle){
			pControlsStyle = new CStdStringPtrMap();
			m_mStyles.Insert(pStylesName,(LPVOID)pControlsStyle);
		}

		LPVOID pStyle = pControlsStyle->Find(pStrStyleName);
		if(pStyle)
			return false;
		return NULL != pControlsStyle->Insert(pStrStyleName,(LPVOID)_StyleMap);
	}


	bool CPaintManagerUI::SetControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlsStyle;
		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle)
		{
			pControlsStyle = new CStdStringPtrMap();
			m_mStyles.Insert(pStylesName,(LPVOID)pControlsStyle);
		}

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));

		if(!pPtrMap){
			pPtrMap = new CStdStringPtrMap();
			pControlsStyle->Insert(pStrStyleName,(LPVOID)pPtrMap);
		}

		if(pPtrMap)
		{
			CDuiString* nVal = new CDuiString(pStrVal);

			if( pPtrMap->Find(pStrKey))
				delete pPtrMap->Find(pStrKey);

			pPtrMap->Insert(pStrKey,nVal);
			return true;
		}
		return false;
	}


	bool CPaintManagerUI::SetControlStyle( LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlsStyle = NULL;
		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle){
			pControlsStyle = new CStdStringPtrMap();
			m_mStyles.Insert(pStylesName,(LPVOID)pControlsStyle);
		}

		LPVOID pStyle = pControlsStyle->Find(pStrStyleName);
		if(pStyle)
			RemoveControlStyle(pStrStyleName,NULL,pStylesName);

		pControlsStyle->Set(pStrStyleName,(LPVOID)_StyleMap);
		return true;
	}


	UiLib::CDuiString CPaintManagerUI::GetControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStylesName /*= NULL*/ )
	{
		if(pStrStyleName == NULL || pStrKey == NULL)
			return _T("");

		CStdStringPtrMap* pPtrMap = static_cast<CStdStringPtrMap*>(GetControlStyles(pStrStyleName,pStylesName));
		if(NULL == pPtrMap)
			return _T("");

		CDuiString* nStrVal = static_cast<CDuiString*>(pPtrMap->Find(pStrKey));
		return NULL == nStrVal?_T(""):nStrVal->GetData();
	}


	CStdStringPtrMap* CPaintManagerUI::GetControlsStyles( LPCTSTR pStylesName ) 
	{
		CStdStringPtrMap *pControlsStyle;

		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle)
			return NULL;

		return pControlsStyle;
	}


	CStdStringPtrMap* CPaintManagerUI::GetControlStyles( LPCTSTR pStrStyleName,LPCTSTR pStylesName /*= NULL*/ ) 
	{
		CStdStringPtrMap *pControlStyle = NULL;
		if(pStylesName)
			pControlStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlStyle)
			return NULL;

		if(pControlStyle->GetSize() == 0 || pStrStyleName == NULL)
			return NULL;

		CStdStringPtrMap* pChildControlStyle = static_cast<CStdStringPtrMap*>(pControlStyle->Find(pStrStyleName));
		return pChildControlStyle?pChildControlStyle:NULL;
	}


	bool CPaintManagerUI::RemoveControlStyle( LPCTSTR pStrStyleName,LPCTSTR pStrKey /*= NULL*/,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlStyle = NULL;
		if(pStrStyleName == NULL)
			return false;

		if(pStylesName)
			pControlStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlStyle)
			return false;

		CStdStringPtrMap* pPtrMap = GetControlStyles(pStrStyleName,pStylesName);

		if(pStrKey == NULL && pControlStyle->Remove(pStrStyleName)){
			delete pPtrMap;
			pPtrMap = NULL;
			return true;
		}

		if(pPtrMap){
			CDuiString* pVal = static_cast<CDuiString*>(pPtrMap->Find(pStrKey));
			if(pVal && pPtrMap->Remove(pStrKey)){
				delete pVal;
				pVal = NULL;
				return true;
			}
		}
		return false;
	}

	void CPaintManagerUI::RemoveAllControlStyle( LPCTSTR pStrStyleName /*= NULL*/,LPCTSTR pStylesName /*= NULL*/ )
	{
		CStdStringPtrMap *pControlsStyle = NULL;
		if(pStylesName)
			pControlsStyle = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));

		if(!pControlsStyle)
			return;

		if(pStrStyleName){
			CStdStringPtrMap* pPtrMap = GetControlStyles(pStrStyleName,pStylesName);
			if(!pPtrMap)
				return;

			CDuiString* pVal = NULL;
			for( int i = 0; i< pPtrMap->GetSize(); i++ ) {
				if(LPCTSTR key = pPtrMap->GetAt(i)) {
					pVal = static_cast<CDuiString*>(pPtrMap->Find(key));
					if(pVal)
						delete pVal;
					pVal = NULL;
				}
			}
			pPtrMap->RemoveAll();
			delete pPtrMap;
			pPtrMap = NULL;
			return;
		}

		for( int i = 0; i< pControlsStyle->GetSize(); i++ ) {
			if(LPCTSTR key = pControlsStyle->GetAt(i)) {
				RemoveAllControlStyle(key,pStylesName);
			}
		}
		pControlsStyle->RemoveAll();
		delete pControlsStyle;
		pControlsStyle = NULL;
	}

	bool CPaintManagerUI::SetCurStyles( LPCTSTR pStylesName /*= NULL*/,bool _NowUpdate /*= true*/ )
	{
		if(!pStylesName){
			m_sCurStylesName.Empty();
			if(_NowUpdate)
				::PostMessage(m_hWndPaint,WM_RELOADSTYLE,NULL,NULL);
			return true;
		}

		if(pStylesName){
			CStdStringPtrMap* pStyleMap = static_cast<CStdStringPtrMap*>(m_mStyles.Find(pStylesName));
			if(pStyleMap){
				m_sCurStylesName	= pStylesName;
				if(_NowUpdate)
					::PostMessage(m_hWndPaint,WM_RELOADSTYLE,NULL,NULL);
				return true;
			}
		}

		return false;
	}


	bool CPaintManagerUI::SetCurStyles( int _iStyleIndex /*= 0*/,bool _NowUpdate /*= true*/ )
	{
		if(_iStyleIndex < 0)
			return false;

		return SetCurStyles(m_mStyles.GetAt(_iStyleIndex),_NowUpdate);
	}


	UINT CPaintManagerUI::GetStylesCount()
	{
		return (UINT)m_mStyles.GetSize();
	}


	UiLib::CDuiString CPaintManagerUI::GetCurStylesName()
	{
		return m_sCurStylesName.GetData();
	}


	bool CPaintManagerUI::RemoveStyles( LPCTSTR pStylesName )
	{
		if(m_sCurStylesName == pStylesName)
			return false;

		for(int nIndex = 0;nIndex < m_mStyles.GetSize();nIndex++)
		{
			CDuiString nKey = m_mStyles.GetAt(nIndex);
			if(nKey != pStylesName)
				continue;

			RemoveAllControlStyle(NULL,nKey.GetData());
			return true;
		}
		return false;
	}


	void CPaintManagerUI::RemoveAllStyles()
	{
		m_sCurStylesName.Empty();

		int isize = m_mStyles.GetSize()-1;
		for(int nIndex = isize;nIndex >= 0;nIndex--)
		{
			CDuiString nKey = m_mStyles.GetAt(nIndex);
			RemoveStyles(nKey.GetData());
		}
		m_mStyles.RemoveAll();
	}

	void CPaintManagerUI::MessageLoop()
	{
		DWORD    dwRet = 0;
		DWORD&   nCount = m_WaitForHandlesMgr.m_nHandleCount;
		HANDLE*& pHandles = m_WaitForHandlesMgr.m_pHandles;
		MSG msg = { 0 };
		while(1)
		{
			dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, FALSE, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
			if (nCount > 0 && dwRet < nCount)
				m_WaitForHandlesMgr.Do(pHandles[dwRet]);
			else
			{
				while( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
				{
					if (WM_QUIT == msg.message)  
					{
						::PostMessage(NULL,WM_QUIT,0,0);
						return;
					}
					if( !CPaintManagerUI::TranslateMessage(&msg) ) 
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
				}
			}
		}
	}

	void CPaintManagerUI::Term()
	{
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
			m_pStrResourceZip.Empty();
		}
	}

	CControlUI* CPaintManagerUI::GetFocus() const
	{
		return m_pFocus;
	}

	void CPaintManagerUI::SetFocus(CControlUI* pControl)
	{
		// Paint manager window has focus?
		HWND hFocusWnd = ::GetFocus();
		if( hFocusWnd != m_hWndPaint && pControl != m_pFocus)
			::SetFocus(m_hWndPaint);
		// Already has focus?
		if( pControl == m_pFocus ) return;
		//set focus to NULL,ignor it;
		if( hFocusWnd == m_hWndPaint && pControl == NULL) 
			return ;
		// Remove focus from old control
		if( m_pFocus != NULL && m_pFocus != pControl) 
		{
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, _T("killfocus"));
			m_pFocus = NULL;
		}
		if( pControl == NULL ) return;
		// Set focus to new control
		if( pControl != NULL 
			&& pControl->GetManager() == this 
			&& pControl->IsVisible() 
			&& pControl->IsEnabled() ) 
		{
			m_pFocus = pControl;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, _T("setfocus"));
		}
	}

	void CPaintManagerUI::SetFocusNeeded(CControlUI* pControl)
	{
		::SetFocus(m_hWndPaint);
		if( pControl == NULL ) return;
		if( m_pFocus != NULL ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, _T("killfocus"));
			m_pFocus = NULL;
		}
		FINDTABINFO info = { 0 };
		info.pFocus = pControl;
		info.bForward = false;
		m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		m_bFocusNeeded = true;
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
	}

	void CPaintManagerUI::AddStringRess(CDuiString szID,CDuiString szText)
	{
		m_StringRess.insert(make_pair(szID,szText));
	}

	CDuiString CPaintManagerUI::GetStringFromID(CDuiString szID)
	{
		CDuiString szValue = m_StringRess[szID];
		return szValue;
	}

	void CPaintManagerUI::ClearStringRess()
	{
		m_StringRess.clear();
	}

	bool CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse)
	{
		ASSERT(pControl!=NULL);
		ASSERT(uElapse>0);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID ) {
					if( pTimer->bKilled == true ) {
						if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) {
							pTimer->bKilled = false;
							return true;
						}
						return false;
					}
					return false;
			}
		}

		m_uTimerID = (++m_uTimerID) % 0xFF;
		if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
		TIMERINFO* pTimer = new TIMERINFO;
		if( pTimer == NULL ) return FALSE;
		pTimer->hWnd = m_hWndPaint;
		pTimer->pSender = pControl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = m_uTimerID;
		pTimer->bKilled = false;
		return m_aTimers.Add(pTimer);
	}

	bool CPaintManagerUI::KillTimer(CControlUI* pControl, UINT nTimerID)
	{
		ASSERT(pControl!=NULL);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID )
			{
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = true;
					return true;
				}
			}
		}
		return false;
	}

	void CPaintManagerUI::KillTimer(CControlUI* pControl)
	{
		ASSERT(pControl!=NULL);
		int count = m_aTimers.GetSize();
		for( int i = 0, j = 0; i < count; i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
			if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				delete pTimer;
				m_aTimers.Remove(i - j);
				j++;
			}
		}
	}

	void CPaintManagerUI::RemoveAllTimers()
	{
		for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
				}
				delete pTimer;
			}
		}

		m_aTimers.Empty();
	}

	void CPaintManagerUI::AddWndContainer(CControlUI *pControl)
	{
		if(!pControl) return;
		m_aWndContainers.Add(pControl);
	}

	void CPaintManagerUI::RemoveWndContainer(CControlUI *pControl)
	{
		if(!pControl) return;
		int count = m_aWndContainers.GetSize();
		for( int i = 0; i < count; i++ ) 
		{
			CControlUI *pTmp = static_cast<CControlUI*>(m_aWndContainers[i]);
			if(pTmp == pControl)
				m_aWndContainers.Remove(i);
		}
	}

	void CPaintManagerUI::RemoveAllWndContainer()
	{
		m_aWndContainers.Empty();
	}

	void CPaintManagerUI::SetCapture(CControlUI* pControl)
	{
		::SetCapture(m_hWndPaint);
		if(pControl&&pControl->IsWantCapture())
			m_pCapture = pControl;
		m_bMouseCapture = true;
	}

	void CPaintManagerUI::ReleaseCapture()
	{
		::ReleaseCapture();
		m_pCapture = NULL;
		m_bMouseCapture = false;
	}

	bool CPaintManagerUI::IsCaptured()
	{
		return m_bMouseCapture;
	}

	CControlUI* CPaintManagerUI::GetCapture() const
	{
		return m_pCapture;
	}

	bool CPaintManagerUI::SetDefaultFocus(CControlUI *pControl)
	{
		m_pDefaultFocus = pControl;
		return true;
	}

	bool CPaintManagerUI::SetNextTabControl(bool bForward)
	{
		// If we're in the process of restructuring the layout we can delay the
		// focus calulation until the next repaint.
		if( m_bUpdateNeeded ) 
		{
			m_bFocusNeeded = true;
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
			return true;
		}
		CDuiString sNextTabName;

		if(m_pFocus)
		{
			if(bForward)
				sNextTabName = m_pFocus->GetNextTabSibling();
			else
				sNextTabName = m_pFocus->GetPrevTabSibline();
			if(!sNextTabName.IsEmpty())
			{
				CControlUI *pControl = FindSubControlByName(NULL,sNextTabName);
				if(pControl)
				{
					SetFocus(pControl);
					m_bFocusNeeded = false;
					return true;
				}
			}
		}

		// Find next/previous tabbable control
		FINDTABINFO info1 = { 0 };
		info1.pFocus = m_pFocus;
		info1.bForward = bForward;
		CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		if( pControl == NULL ) {  
			if( bForward ) {
				// Wrap around
				FINDTABINFO info2 = { 0 };
				info2.pFocus = bForward ? NULL : info1.pLast;
				info2.bForward = bForward;
				pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
			}
			else {
				pControl = info1.pLast;
			}
		}
		if( pControl != NULL ) SetFocus(pControl);
		m_bFocusNeeded = false;
		return true;
	}

	void CPaintManagerUI::EventAllControl( TEventUI& event,CControlUI* pControl /*= NULL*/ )
	{
		IContainerUI* pContainer = static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")));
		if(pContainer){
			int nCountNode = pContainer->GetCount();
			for(int nIndex = 0;nIndex < nCountNode;nIndex++)
			{
				CControlUI* nControl = pContainer->GetItemAt(nIndex);
				if(nControl)
					nControl->Event(event);

				EventAllControl(event,nControl);
			}
		}
	}

	bool CPaintManagerUI::AddNotifier(INotifyUI* pNotifier)
	{
		ASSERT(m_aNotifiers.Find(pNotifier)<0);
		return m_aNotifiers.Add(pNotifier);
	}

	bool CPaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
	{
		for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
			if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
				return m_aNotifiers.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
		return m_aPreMessageFilters.Add(pFilter);
	}

	bool CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
				return m_aPreMessageFilters.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(m_aMessageFilters.Find(pFilter)<0);
		return m_aMessageFilters.Add(pFilter);
	}

	bool CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
				return m_aMessageFilters.Remove(i);
			}
		}
		return false;
	}

	int CPaintManagerUI::GetPostPaintCount() const
	{
		return m_aPostPaintControls.GetSize();
	}

	bool CPaintManagerUI::AddPostPaint(CControlUI* pControl)
	{
		ASSERT(m_aPostPaintControls.Find(pControl) < 0);
		return m_aPostPaintControls.Add(pControl);
	}

	bool CPaintManagerUI::RemovePostPaint(CControlUI* pControl)
	{
		for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
			if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
				return m_aPostPaintControls.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
	{
		RemovePostPaint(pControl);
		return m_aPostPaintControls.InsertAt(iIndex, pControl);
	}

	void CPaintManagerUI::AddDelayedCleanup(CControlUI* pControl)
	{
		pControl->SetManager(this, NULL, false);
		m_aDelayedCleanup.Add(pControl);
		::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
	}

	void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
	{
		TNotifyUI Msg;
		Msg.pSender = pControl;
		Msg.sType = pstrMessage;
		Msg.wParam = wParam;
		Msg.lParam = lParam;
		SendNotify(Msg, bAsync);
	}

	void CPaintManagerUI::SendNotify(TNotifyUI& Msg, bool bAsync /*= false*/)
	{
		Msg.ptMouse = m_ptLastMousePos;
		Msg.dwTimestamp = ::GetTickCount();
		if( m_bUsedVirtualWnd )
		{
			Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
		}
		if( !bAsync ) {
			if( Msg.pSender != NULL ) {
				if( Msg.pSender->OnNotify ){
					if(Msg.pSender->OnNotify(&Msg))
						return;
				}
			}
			for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
				static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
			}
		}
		else {
			TNotifyUI *pMsg = new TNotifyUI;
			pMsg->pSender = Msg.pSender;
			pMsg->sType = Msg.sType;
			pMsg->wParam = Msg.wParam;
			pMsg->lParam = Msg.lParam;
			pMsg->ptMouse = Msg.ptMouse;
			pMsg->dwTimestamp = Msg.dwTimestamp;
			m_aAsyncNotify.Add(pMsg);
		}
	}

	bool CPaintManagerUI::UseParentResource(CPaintManagerUI* pm)
	{
		if( pm == NULL ) {
			m_pParentResourcePM = NULL;
			return true;
		}
		if( pm == this ) return false;

		CPaintManagerUI* pParentPM = pm->GetParentResource();
		while( pParentPM ) {
			if( pParentPM == this ) return false;
			pParentPM = pParentPM->GetParentResource();
		}
		m_pParentResourcePM = pm;
		return true;
	}

	CPaintManagerUI* CPaintManagerUI::GetParentResource() const
	{
		return m_pParentResourcePM;
	}

	DWORD CPaintManagerUI::GetDefaultDisabledColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultDisabledColor();
		return m_dwDefaultDisabledColor;
	}

	void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor)
	{
		m_dwDefaultDisabledColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontColor();
		return m_dwDefaultFontColor;
	}

	void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor)
	{
		m_dwDefaultFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkFontColor();
		return m_dwDefaultLinkFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor)
	{
		m_dwDefaultLinkFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultLinkHoverFontColor();
		return m_dwDefaultLinkHoverFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor)
	{
		m_dwDefaultLinkHoverFontColor = dwColor;
	}

	DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultSelectedBkColor();
		return m_dwDefaultSelectedBkColor;
	}

	void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor)
	{
		m_dwDefaultSelectedBkColor = dwColor;
	}

	TFontInfo* CPaintManagerUI::GetDefaultFontInfo()
	{
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultFontInfo();
		return m_pRenderCore->GetDefaultFontInfo();
	}

	void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		if(m_pRenderCore)
			m_pRenderCore->SetDefaultFont(pStrFontName,nSize,bBold,bUnderline,bItalic);
	}

	DWORD CPaintManagerUI::GetCustomFontCount() const
	{
		return m_aCustomFonts.GetSize();
	}

	TFontInfo* CPaintManagerUI::AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = m_pRenderCore->AddFont(pStrFontName,nSize,bBold,bUnderline,bItalic);
		if( !m_aCustomFonts.Add(pFontInfo) ) 
		{
			SAFE_DELETE(pFontInfo);
			return NULL;
		}
		return pFontInfo;
	}

	TFontInfo* CPaintManagerUI::AddFontAt(int index, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = m_pRenderCore->AddFont(pStrFontName,nSize,bBold,bUnderline,bItalic);
		if( !m_aCustomFonts.InsertAt(index, pFontInfo) ) {
			SAFE_DELETE(pFontInfo);
			return NULL;
		}
		return pFontInfo;
	}

	void CPaintManagerUI::AddFontInfo(TFontInfo* pFontInfo)
	{
		if(!pFontInfo) return;
		m_aCustomFonts.Add(pFontInfo);
	}

	TFontInfo* CPaintManagerUI::GetFont(int index)
	{
		TFontInfo* pFontInfo = NULL;
		if( index < 0 || index >= m_aCustomFonts.GetSize() ) return GetDefaultFontInfo();
		pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
		return pFontInfo;
	}

	TFontInfo* CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( _tcscmp(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		return NULL;
	}

	bool CPaintManagerUI::FindFont(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) return true;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(hFont);
		return false;
	}

	bool CPaintManagerUI::FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( _tcscmp(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return true;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->FindFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		return false;
	}

	int CPaintManagerUI::GetFontIndex(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) return it;
		}
		return -1;
	}

	int CPaintManagerUI::GetFontIndex(TFontInfo *pFontInfoFd)
	{
		if(!pFontInfoFd) return -1;
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo == pFontInfoFd ) return it;
		}
		return -1;
	}

	int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( _tcscmp(pFontInfo->sFontName,pStrFontName) == 0 && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return it;
		}
		return -1;
	}

	bool CPaintManagerUI::RemoveFont(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) 
			{
				SAFE_DELETE(pFontInfo);
				return m_aCustomFonts.Remove(it);
			}
		}
		return false;
	}

	bool CPaintManagerUI::RemoveFont(TFontInfo *pFontInfoRm)
	{
		if(!pFontInfoRm)	return false;
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo == pFontInfoRm ) 
			{
				SAFE_DELETE(pFontInfo);
				return m_aCustomFonts.Remove(it);
			}
		}
		return false;
	}

	bool CPaintManagerUI::RemoveFontAt(int index)
	{
		if( index < 0 || index >= m_aCustomFonts.GetSize() ) return false;
		TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
		SAFE_DELETE(pFontInfo);
		return m_aCustomFonts.Remove(index);
	}

	void CPaintManagerUI::RemoveAllFonts()
	{
		TFontInfo* pFontInfo;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ )
		{
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			SAFE_DELETE(pFontInfo);
		}
		m_aCustomFonts.Empty();
	}

	TFontInfo* CPaintManagerUI::GetFontInfo(int index)
	{
		TFontInfo* pFontInfo = NULL;
		if( index >= 0 && index < m_aCustomFonts.GetSize() ) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
			if(pFontInfo && pFontInfo->tm.tmHeight == 0 ) 
			{
				HDC hDCWnd = m_pRenderCore->GetRenderHDC(FALSE);
				HFONT hOldFont = (HFONT) ::SelectObject(hDCWnd, pFontInfo->hFont);
				::GetTextMetrics(hDCWnd, &pFontInfo->tm);
				::SelectObject(hDCWnd, hOldFont);
			}
			return pFontInfo;
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetFontInfo(index);
		return GetDefaultFontInfo();
	}

	TFontInfo* CPaintManagerUI::GetFontInfo(HFONT hFont)
	{
		TFontInfo* pFontInfo = NULL;
		for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
			if( pFontInfo->hFont == hFont ) {
				if( pFontInfo->tm.tmHeight == 0 ) 
				{
					HDC hDCWnd = m_pRenderCore->GetRenderHDC(FALSE);
					HFONT hOldFont = (HFONT) ::SelectObject(hDCWnd, pFontInfo->hFont);
					::GetTextMetrics(hDCWnd, &pFontInfo->tm);
					::SelectObject(hDCWnd, hOldFont);
				}
				return pFontInfo;
			}
		}
		if( m_pParentResourcePM ) return m_pParentResourcePM->GetFontInfo(hFont);
		return GetDefaultFontInfo();
	}

	const TImageInfo* CPaintManagerUI::GetImage(LPCTSTR bitmap)
	{
		TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
		if( !data && m_pParentResourcePM )
			return m_pParentResourcePM->GetImage(bitmap);
		else return data;
	}

	const TImageInfo* CPaintManagerUI::GetImage( CDuiImage& bitmap )
	{
		TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap.GetImagePath()));
		if( !data && m_pParentResourcePM )
			return m_pParentResourcePM->GetImage(bitmap);
		else return data;
	}

	const TImageInfo* CPaintManagerUI::GetImageEx(const CDuiImage& bitmap, LPCTSTR type, DWORD mask)
	{
		CDuiImage& mBitmap = (CDuiImage&)bitmap;
		TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(mBitmap.GetImagePath().GetData()));
		if( !data ) 
			data = (TImageInfo*)AddImage(mBitmap, type, mask);
		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage( CDuiImage& bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/ )
	{
		if(!m_pRenderCore) return NULL;
		TImageInfo* data = NULL;
		if( type != NULL )
		{
			if(!bitmap.GetRes().IsEmpty()) 
			{
				LPTSTR pstr = NULL;
				int iIndex = _tcstol(bitmap.GetRes().GetData(), &pstr, 10);
				data = m_pRenderCore->LoadImage(iIndex,type,mask);
			}
		}
		else
			data = m_pRenderCore->LoadImage(bitmap.GetImagePath().GetData(),NULL, mask);

		if( !data ) return NULL;
		if( type != NULL ) data->sResType = type;
		data->dwMask = mask;
		if( !m_mImageHash.Insert(bitmap.GetImagePath().GetData(), data) ) 
		{
			::DeleteObject(data->hBitmap);
			delete data;
		}
		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage( CDuiImage& bitmap, HBITMAP hBitmap, int nWidth, int nHeight, bool bAlpha )
	{
		if( hBitmap == NULL || nWidth <= 0 || nHeight <= 0 || !m_pRenderCore ) return NULL;

		TImageInfo* data =m_pRenderCore->LoadImage(hBitmap,nWidth,nHeight,bAlpha);
		if( !m_mImageHash.ForceInsert(bitmap.GetImagePath(), data) ) {
			::DeleteObject(data->hBitmap);
			delete data;
		}
		return data;
	}

	bool CPaintManagerUI::RemoveImage(LPCTSTR bitmap)
	{
		const TImageInfo* data = GetImage(bitmap);
		if( !data ) return false;
		SAFE_DELETE(data);
		return m_mImageHash.Remove(bitmap);
	}

	bool CPaintManagerUI::RemoveImage(CDuiImage& bitmap)
	{
		const TImageInfo* data = GetImage(bitmap);
		if( !data ) return false;
		SAFE_DELETE(data);
		return m_mImageHash.Remove(bitmap);
	}

	void CPaintManagerUI::RemoveAllImages()
	{
		TImageInfo* data;
		for( int i = 0; i< m_mImageHash.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_mImageHash.GetAt(i))
			{
				data = static_cast<TImageInfo*>(m_mImageHash.Find(key, false));
				SAFE_DELETE(data);
			}
		}
		m_mImageHash.RemoveAll();
	}

	void CPaintManagerUI::ReloadAllImages()
	{
		bool bRedraw = false;
		TImageInfo* data;
		TImageInfo* pNewData;
		for( int i = 0; i< m_mImageHash.GetSize(); i++ ) 
		{
			if(LPCTSTR bitmap = m_mImageHash.GetAt(i)) 
			{
				data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
				if( data != NULL ) 
				{
					if( data->sResType != NULL ) 
					{
						if(isdigit(*bitmap) ) 
						{
							LPTSTR pstr = NULL;
							int iIndex = _tcstol(bitmap, &pstr, 10);
							pNewData = m_pRenderCore->LoadImage(iIndex, data->sResType, data->dwMask);
						}
					}
					else
						pNewData = m_pRenderCore->LoadImage(bitmap, NULL, data->dwMask);
					if( pNewData == NULL ) continue;
					SAFE_DELETE(data);
					m_mImageHash.Set(bitmap,pNewData);
					bRedraw = true;
				}
			}
		}
		if( bRedraw && m_pRoot ) m_pRoot->Invalidate();
	}

	void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList)
	{
		CDuiString* pDefaultAttr = new CDuiString(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			if (m_DefaultAttrHash.Find(pStrControlName) == NULL)
				m_DefaultAttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr);
			else
				delete pDefaultAttr;
		}
	}

	LPCTSTR CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(pStrControlName));
		if( !pDefaultAttr && m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultAttributeList(pStrControlName);

		if( pDefaultAttr ) return pDefaultAttr->GetData();
		else return NULL;
	}

	bool CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName)
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_DefaultAttrHash.Remove(pStrControlName);
	}

	const CStdStringPtrMap& CPaintManagerUI::GetDefaultAttribultes() const
	{
		return m_DefaultAttrHash;
	}

	void CPaintManagerUI::RemoveAllDefaultAttributeList()
	{
		CDuiString* pDefaultAttr;
		for( int i = 0; i< m_DefaultAttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_DefaultAttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<CDuiString*>(m_DefaultAttrHash.Find(key));
				delete pDefaultAttr;
			}
		}
		m_DefaultAttrHash.RemoveAll();
	}

	CControlUI* CPaintManagerUI::GetRoot() const
	{
		ASSERT(m_pRoot);
		return m_pRoot;
	}

	CControlUI* CPaintManagerUI::FindControl(POINT pt) const
	{
		ASSERT(m_pRoot);
		return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	CControlUI* CPaintManagerUI::FindControl(LPCTSTR pstrName) const
	{
		ASSERT(m_pRoot);
		return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
	}

	CControlUI* CPaintManagerUI::FindSubControlByPoint(CControlUI* pParent, POINT pt) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	CControlUI* CPaintManagerUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
	}

	CControlUI* CPaintManagerUI::FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Resize(iIndex + 1);
		return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
	}

	CStdPtrArray* CPaintManagerUI::FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Empty();
		pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
		return &m_aFoundControls;
	}

	CStdPtrArray* CPaintManagerUI::GetSubControlsByClass()
	{
		return &m_aFoundControls;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
	{
		CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(pData);
		const CDuiString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		// Add this control to the hash list
		pManager->m_mNameHash.Set(sName, pThis);
		return NULL; // Attempt to add all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
	{
		int* pnCount = static_cast<int*>(pData);
		(*pnCount)++;
		return NULL;  // Count all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
	{
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
	{
		FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
		if( pInfo->pFocus == pThis ) {
			if( pInfo->bForward ) pInfo->bNextIsIt = true;
			return pInfo->bForward ? NULL : pInfo->pLast;
		}
		if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
		pInfo->pLast = pThis;
		if( pInfo->bNextIsIt ) return pThis;
		if( pInfo->pFocus == NULL ) return pThis;
		return NULL;  // Examine all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
	{
		if( !pThis->IsVisible() ) return NULL; 
		FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
		if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
		if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL ) return NULL;   // Labels never get focus!
		return pFS->bPickNext ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromUpdate(CControlUI* pThis, LPVOID pData)
	{
		return pThis->IsUpdateNeeded() ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
		const CDuiString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		CStdPtrArray* pFoundControls = pThis->GetManager()->GetSubControlsByClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
			int iIndex = -1;
			while( pFoundControls->GetAt(++iIndex) != NULL ) ;
			if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
		}
		if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
		return NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
			pThis->GetManager()->GetSubControlsByClass()->Add((LPVOID)pThis);
		return NULL;
	}

	bool CPaintManagerUI::TranslateAccelerator(LPMSG pMsg)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
			return lResult == S_OK;
		}
		return false;
	}

	bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
	{
		// Pretranslate Message takes care of system-wide messages, such as
		// tabbing and shortcut key-combos. We'll look for all messages for
		// each window and any child control attached.
		UINT uStyle = GetWindowStyle(pMsg->hwnd);
		TCHAR strClassName[255] = {0};
		GetClassName(pMsg->hwnd,strClassName,255);
		UINT uChildRes = uStyle & WS_CHILD;	
		LRESULT lRes = 0;
		if(_tcscmp(strClassName,L"EditWnd") == 0 &&  uChildRes != 0)
		{
			HWND hWndParent = ::GetParent(pMsg->hwnd);
			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);        
				if(pMsg->hwnd == pT->GetPaintWindow() || hWndParent == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
			}
		}
		else
		{
			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
				if(pMsg->hwnd == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
			}
		}
		return false;
	}


	bool CPaintManagerUI::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
		return m_aTranslateAccelerator.Add(pTranslateAccelerator);
	}

	bool CPaintManagerUI::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i]) == pTranslateAccelerator)
			{
				return m_aTranslateAccelerator.Remove(i);
			}
		}
		return false;
	}

#ifdef UILIB_D3D
	bool CPaintManagerUI::AddAnimationJob( const CDxAnimationUI& _DxAnimationUI )
	{
		try
		{
			CDxAnimationUI* pDxAnimationJob = new CDxAnimationUI(_DxAnimationUI);
			if( pDxAnimationJob == NULL ) return false;
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
			bool nRet = m_anim.AddJob(pDxAnimationJob);
			return nRet;
		}
		catch (...)
		{
			throw "CPaintManagerUI::AddAnimationJob";
		}
	}
#endif

	HPEN CPaintManagerUI::GetThemePen( UITYPE_COLOR Index ) const
	{
		try
		{
			if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
			if( m_hPens[Index] == NULL ) m_hPens[Index] = ::CreatePen(PS_SOLID, 1, m_clrColors[Index][0]);
			return m_hPens[Index];
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemePen";
		}
	}

	HFONT CPaintManagerUI::GetThemeFont( UITYPE_FONT Index ) const
	{
		try
		{
			if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return NULL;
			if( m_hFonts[Index] == NULL ) m_hFonts[Index] = ::CreateFontIndirect(&m_aLogFonts[Index]);
			return m_hFonts[Index];
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeFont";
		}
	}

	HBRUSH CPaintManagerUI::GetThemeBrush( UITYPE_COLOR Index ) const
	{
		try
		{
			if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
			if( m_hBrushes[Index] == NULL ) m_hBrushes[Index] = ::CreateSolidBrush(m_clrColors[Index][0]);
			return m_hBrushes[Index];
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeBrush";
		}
	}

	COLORREF CPaintManagerUI::GetThemeColor( UITYPE_COLOR Index ) const
	{
		try
		{
			if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return RGB(0,0,0);
			return m_clrColors[Index][0];
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeColor";
		}
	}

	HICON CPaintManagerUI::GetThemeIcon( int Index, int cxySize ) const
	{
		try
		{
			if( m_himgIcons16 == NULL ) {
				m_himgIcons16 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
				m_himgIcons24 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
				m_himgIcons32 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
				m_himgIcons50 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS50), 50, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			}
			if( cxySize == 16 ) return ImageList_GetIcon(m_himgIcons16, Index, ILD_NORMAL);
			else if( cxySize == 24 ) return ImageList_GetIcon(m_himgIcons24, Index, ILD_NORMAL);
			else if( cxySize == 32 ) return ImageList_GetIcon(m_himgIcons32, Index, ILD_NORMAL);
			else if( cxySize == 50 ) return ImageList_GetIcon(m_himgIcons50, Index, ILD_NORMAL);
			return NULL;
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeIcon";
		}
	}

	const TEXTMETRIC& CPaintManagerUI::GetThemeFontInfo( UITYPE_FONT Index ) const
	{
		try
		{
			if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return m_aTextMetrics[0];
			if( m_aTextMetrics[Index].tmHeight == 0 ) 
			{
				HDC hDCWnd = m_pRenderCore->GetRenderHDC(FALSE);
				HFONT hOldFont = (HFONT) ::SelectObject(hDCWnd, GetThemeFont(Index));
				::GetTextMetrics(hDCWnd, &m_aTextMetrics[Index]);
				::SelectObject(hDCWnd, hOldFont);
			}
			return m_aTextMetrics[Index];
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeFontInfo";
		}
	}

	bool CPaintManagerUI::GetThemeColorPair( UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2 ) const
	{
		try
		{
			if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return false;
			clr1 = m_clrColors[Index][0];
			clr2 = m_clrColors[Index][1];
			return true;
		}
		catch (...)
		{
			throw "CPaintManagerUI::GetThemeColorPair";
		}
	}

	void CPaintManagerUI::UsedVirtualWnd(bool bUsed)
	{
		m_bUsedVirtualWnd = bUsed;
	}

	bool CPaintManagerUI::AddPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		return SetPropertyAction(pAGroupName,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,true);
	}

	bool CPaintManagerUI::AddPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		return SetPropertyAction(tAGroup,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,true);
	}

	bool CPaintManagerUI::SetPropertyAction( TAGroup& tAGroup,TProperty* tProperty,bool bAutoCreate /*= false*/ )
	{
		if(!tProperty)
			return false;

		int nIndex = -1;
		TProperty* pTProperty = GetPropertyAction(tAGroup,tProperty->sName.GetData(),tProperty->sType.GetData(),nIndex);
		if(!pTProperty && !bAutoCreate)
			return false;

		if(pTProperty){
			delete pTProperty;
			pTProperty = NULL;
		}

		tAGroup.mPropertys.Remove(nIndex);
		return tAGroup.mPropertys.Add(tProperty);
	}

	bool CPaintManagerUI::SetPropertyAction( LPCTSTR pAGroupName,TProperty* tProperty,bool bAutoCreate /*= false*/ )
	{
		if(!tProperty)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		return SetPropertyAction(*pTAGroup,tProperty,bAutoCreate);
	}

	bool CPaintManagerUI::SetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/,bool bAutoCreate /*= false*/ )
	{
		if(!pAGroupName || !pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		return SetPropertyAction(*pTAGroup,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart,bAutoCreate);
	}

	bool CPaintManagerUI::SetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/,bool bAutoCreate /*= false*/ )
	{
		if(!pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		TProperty* pTProperty = new TProperty();

		if(pTProperty && SetPropertyActionParse(*pTProperty,pPropertyName,pType,pStartValue,pEndValue,iInterval,iTimer,iDelay,bRevers,bLoop,bAutoStart)){
			if(SetPropertyAction(tAGroup.sName.GetData(),pTProperty,bAutoCreate))
				return true;
			delete pTProperty;
			pTProperty = NULL;
		}
		return false;
	}

	TProperty* CPaintManagerUI::GetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType ) const
	{
		int nIndex = -1;
		return GetPropertyAction(pAGroupName,pPropertyName,pType,nIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType ) const
	{
		int nIndex = -1;
		return GetPropertyAction(tAGroup,pPropertyName,pType,nIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex ) const
	{
		if(!pAGroupName || !pPropertyName)
			return NULL;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return NULL;

		return GetPropertyAction(*pTAGroup,pPropertyName,pType,iIndex);
	}

	TProperty* CPaintManagerUI::GetPropertyAction( TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex ) const
	{
		if(!pPropertyName || !pType)
			return NULL;

		iIndex = -1;

		for(int i = 0;i < tAGroup.mPropertys.GetSize();i++){
			TProperty* nTProperty = tAGroup.mPropertys.GetAt(i);
			if(nTProperty && _tcscmp(nTProperty->sName.GetData(),pPropertyName) == 0 && _tcscmp(nTProperty->sType.GetData(),pType) == 0){
				iIndex = i;
				return nTProperty;
			}
			break;
		}
		return NULL;
	}

	bool CPaintManagerUI::RemovePropertyAction( LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType )
	{
		if(!pAGroupName || !pPropertyName)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		int nIndex = -1;
		TProperty* pTProperty = GetPropertyAction(*pTAGroup,pPropertyName,pType,nIndex);
		if(!pTProperty)
			return true;

		delete pTProperty;
		return pTAGroup->mPropertys.Remove(nIndex);
	}

	bool CPaintManagerUI::SetPropertyActionParse( TProperty& nTProperty,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop /*= false*/,bool bAutoStart /*= true*/ )
	{
		if(!pPropertyName || !pType || (!pStartValue && !pEndValue))
			return false;

		if( _tcscmp(pType, _T("int")) == 0 || _tcscmp(pType, _T("image.fade")) == 0 ) {
			nTProperty.nStartValue.iValue = _ttoi(pStartValue);
			nTProperty.nEndValue.iValue = _ttoi(pEndValue);
		}
		else if( _tcscmp(pType, _T("color")) == 0 || _tcscmp(pType, _T("image.mask")) == 0 ) {
			if( *pStartValue == _T('#')) pStartValue = ::CharNext(pStartValue);
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.dwValue = _tcstoul(pStartValue, &pStartStr, 16);

			if( *pEndValue == _T('#')) pEndValue = ::CharNext(pEndValue);
			LPTSTR pEndStr = NULL;
			nTProperty.nEndValue.dwValue = _tcstoul(pEndValue, &pEndStr, 16);
		}
		else if( _tcscmp(pType, _T("rect")) == 0 || _tcscmp(pType, _T("image.source")) == 0 || _tcscmp(pType, _T("image.corner")) == 0 || _tcscmp(pType, _T("image.dest")) == 0 ) {
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.rcValue.left = _tcstol(pStartValue, &pStartStr, 10);		ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.top = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.right = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.rcValue.bottom = _tcstol(pStartStr + 1, &pStartStr, 10);	ASSERT(pStartStr); 

			LPTSTR pEndStr = NULL;
			nTProperty.nEndValue.rcValue.left = _tcstol(pEndValue, &pEndStr, 10);		ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.top = _tcstol(pEndStr + 1, &pEndStr, 10);    ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.right = _tcstol(pEndStr + 1, &pEndStr, 10);  ASSERT(pEndStr);
			nTProperty.nEndValue.rcValue.bottom = _tcstol(pEndStr + 1, &pEndStr, 10); ASSERT(pEndStr); 
		}
		else if( _tcscmp(pType, _T("point")) == 0 ) {
			LPTSTR pStartStr = NULL;
			nTProperty.nStartValue.siValue.cx = _tcstol(pStartValue, &pStartStr, 10);	ASSERT(pStartStr);
			nTProperty.nStartValue.siValue.cy  = _tcstol(pStartStr + 1, &pStartStr, 10); ASSERT(pStartStr);

			LPTSTR pEndStr = NULL;
			nTProperty.nStartValue.siValue.cx = _tcstol(pEndValue, &pEndStr, 10);	ASSERT(pEndStr);
			nTProperty.nStartValue.siValue.cy  = _tcstol(pEndStr + 1, &pEndStr, 10); ASSERT(pEndStr);
		}
		else false;

		if(_tcscmp(_T("none"),pStartValue) == 0){
			::ZeroMemory((void*)&nTProperty.nStartValue, sizeof(TProperty::unValue));
			nTProperty.sStartValue = _T("none");
		}
		if(_tcscmp(_T("none"),pEndValue) == 0){
			::ZeroMemory((void*)&nTProperty.nEndValue, sizeof(TProperty::unValue));
			nTProperty.sEndValue = _T("none");
		}

		nTProperty.sName		= pPropertyName;
		nTProperty.sType		= pType;
		nTProperty.bLoop		= bLoop;
		nTProperty.bReverse		= bRevers;
		nTProperty.bAutoStart	= bAutoStart;
		nTProperty.uInterval	= iInterval < 0?0:iInterval;
		nTProperty.uTimer		= iTimer < iInterval ?iInterval:iTimer;
		nTProperty.uDelay		= iDelay < 0?0:iDelay;

		return true;
	}

	int CPaintManagerUI::HasPropertyMsgType( TAGroup& tAGroup,LPCTSTR pType )
	{
		if(!pType)
			return -1;

		for(int i = 0;i < tAGroup.mPropertys.GetSize();i++){
			TProperty* pTProperty = tAGroup.mPropertys.GetAt(i);
			if(!pTProperty)
				continue;

			if(_tcscmp(pType,pTProperty->sType.GetData()) == 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasPropertyMsgType( LPCTSTR pAGroupName,LPCTSTR pType )
	{
		if(!pAGroupName || !pType)
			return -1;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasPropertyMsgType(*pTAGroup,pType);
	}

	bool CPaintManagerUI::AddActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/ )
	{
		if(!pAGroupName || !pNotifyName || GetActionScriptGroup(pAGroupName))
			return false;

		TAGroup* pTAGroup			= new TAGroup();
		pTAGroup->sName				= pAGroupName;
		pTAGroup->sMsgType			= _T("notify");
		pTAGroup->sMsgValue			= pNotifyName;
		pTAGroup->iEventValue		= UIEVENT__ALL;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		m_mActionScript.Set(pAGroupName,pTAGroup);
		return true;
	}

	bool CPaintManagerUI::AddActionScriptGroup( LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/ )
	{
		if(!pAGroupName || pEventType == UIEVENT__ALL || GetActionScriptGroup(pAGroupName))
			return false;

		TAGroup* pTAGroup			= new TAGroup();
		pTAGroup->sName				= pAGroupName;
		pTAGroup->sMsgType			= _T("event");
		pTAGroup->sMsgValue.Empty();
		pTAGroup->iEventValue		= pEventType;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		m_mActionScript.Set(pAGroupName,pTAGroup);
		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( TAGroup** tAGroup,bool bMergerProperty /*= false*/ )
	{
		if(!*tAGroup)
			return false;

		TAGroup* pTAGroup = m_mActionScript.Find((*tAGroup)->sName.GetData());

		if(!pTAGroup)
			return false;

		if(bMergerProperty){
			for(int i = 0;i < (*tAGroup)->mPropertys.GetSize();i++){
				TProperty* pTProperty = (*tAGroup)->mPropertys.GetAt(i);
				if(!pTProperty)
					continue;

				SetPropertyAction(*pTAGroup,pTProperty,true);
			}
			for(int i = 0;i < pTAGroup->mPropertys.GetSize();i++){
				TProperty* pTProperty = pTAGroup->mPropertys.GetAt(i);
				if(!pTProperty)
					continue;

				SetPropertyAction(**tAGroup,pTProperty,true);
			}
		}

		pTAGroup = m_mActionScript.Set((*tAGroup)->sName.GetData(),(*tAGroup));
		if(!pTAGroup)
			return true;

		delete pTAGroup;
		pTAGroup = NULL;

		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/,bool bMergerProperty /*= false*/ )
	{
		if(!pAGroupName || !pNotifyName)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		pTAGroup->sMsgType = _T("notify");
		pTAGroup->sMsgValue= pNotifyName;
		pTAGroup->iEventValue		= UIEVENT__ALL;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		return true;
	}

	bool CPaintManagerUI::SetActionScriptGroup( LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval /*= 0*/,int iDefaultTimer /*= 500*/,bool bDefaultReverse /*= false*/,bool bDefaultLoop /*= false*/,bool bDefaultAutoStart /*= true*/,bool bMergerProperty /*= false*/ )
	{
		if(!pAGroupName || !pEventType)
			return false;

		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return false;

		pTAGroup->sMsgType = _T("notify");
		pTAGroup->sMsgValue.Empty();
		pTAGroup->iEventValue		= pEventType;
		pTAGroup->uDefaultInterval	= iDefaultInterval < 0?0:iDefaultInterval;
		pTAGroup->uDefaultTimer		= iDefaultTimer < (int)pTAGroup->uDefaultInterval?pTAGroup->uDefaultInterval:iDefaultTimer;
		pTAGroup->bDefaultLoop		= bDefaultLoop;
		pTAGroup->bDefaultAutoStart	= bDefaultAutoStart;
		pTAGroup->bDefaultReverse	= bDefaultReverse;

		return true;
	}

	TAGroup* CPaintManagerUI::GetActionScriptGroup( LPCTSTR pAGroupName ) const
	{
		if(!pAGroupName)
			return NULL;

		return m_mActionScript.Find(pAGroupName);
	}

	bool CPaintManagerUI::RemoveActionScriptGroup( LPCTSTR pAGroupName )
	{
		if(!pAGroupName)
			return false;

		TAGroup* pTAGroup = m_mActionScript.Find(pAGroupName);
		if(!pTAGroup)
			return false;

		if(!m_mActionScript.Remove(pAGroupName))
			return false;

		delete pTAGroup;
		pTAGroup = NULL;
		return true;
	}

	void CPaintManagerUI::RemoveActionScriptGroupAll()
	{
		int isize = m_mActionScript.GetSize() -1;
		for(int i = isize;i >= 0;i--){
			LPCTSTR key = m_mActionScript.GetAt(i);
			TAGroup* pTAGroup = m_mActionScript.Find(key);
			if(!pTAGroup)
				continue;

			int jsize=pTAGroup->mPropertys.GetSize()-1;
			for(int j=jsize;j>=0;j--)
			{
				pTAGroup->mPropertys.Remove(j,true);
			}

			delete pTAGroup;
			pTAGroup = NULL;
			m_mActionScript.Remove(key);
		}
		m_mActionScript.RemoveAll();
	}

	int CPaintManagerUI::HasActionScriptGroup( TAGroup& tAGroup,LPCTSTR pMsgType )
	{
		if(!pMsgType)
			return -1;

		for(int i = 0;i < m_mActionScript.GetSize();i++){
			TAGroup* pTAGroup = m_mActionScript.GetAtObj(i);
			if(!pTAGroup)
				continue;

			if(_tcscmp(pMsgType,pTAGroup->sMsgType.GetData()) == 0 && _tcscmp(tAGroup.sName.GetData(),pTAGroup->sName.GetData()) != 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasActionScriptGroup( TAGroup& tAGroup,int pEventValue /*= 0*/ )
	{
		if(!pEventValue)
			return -1;

		for(int i = 0;i < m_mActionScript.GetSize();i++){
			TAGroup* pTAGroup = m_mActionScript.GetAtObj(i);
			if(!pTAGroup)
				continue;

			if(pTAGroup->iEventValue == pEventValue && _tcscmp(tAGroup.sName.GetData(),pTAGroup->sName.GetData()) != 0)
				return 1;
		}
		return 0;
	}

	int CPaintManagerUI::HasActionScriptGroup( LPCTSTR pAGroupName,LPCTSTR pMsgType )
	{
		if(!pMsgType)
			return -1;
		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasActionScriptGroup(*pTAGroup,pMsgType);
	}

	int CPaintManagerUI::HasActionScriptGroup( LPCTSTR pAGroupName,int pEventValue /*= 0*/ )
	{
		if(!pEventValue)
			return -1;
		TAGroup* pTAGroup = GetActionScriptGroup(pAGroupName);
		if(!pTAGroup)
			return -1;

		return HasActionScriptGroup(*pTAGroup,pEventValue);
	}

	CEventSource& CPaintManagerUI::GetEventSource()
	{
		return m_aCustomEvents;
	}

	CDuiTrayIcon& CPaintManagerUI::GetTrayObject()
	{
		return mDuiTray;
	}

	CUIRenderCore * CPaintManagerUI::GetRenderCore()
	{
		return m_pRenderCore;
	}

	void CPaintManagerUI::AddIDOKEvent(const CDelegateBase &d )
	{
		m_aIDOKEvents += d;
	}

	bool CPaintManagerUI::HandleIDOKEvent()
	{
		if(m_aIDOKEvents)
		{
			TNotifyUI notify;
			notify.sType = DUI_MSGTYPE_IDOK;
			m_aIDOKEvents(&notify);
			return true;
		}
		return false;
	}

} // namespace UiLib
