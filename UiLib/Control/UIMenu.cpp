#include "stdafx.h"
#include "UIMenu.h"

namespace UiLib {

	REGIST_DUICLASS(CMenuElementUI);
	REGIST_DUICLASS(CMenuUI);

// 新菜单窗口与父窗口的间隔
#define MENUWIN_INSET_CAP -2

ContextMenuObserver s_context_menu_observer;

CMenuUI::CMenuUI()
{
	m_pBelongWnd = NULL;
	m_pBirthElement = NULL;
	if (GetHeader() != NULL)
		GetHeader()->SetVisible(false);
}

CMenuUI::~CMenuUI()
{
	m_pBirthElement = NULL;
}

LPCTSTR CMenuUI::GetClass() const
{
    return _T("MenuUI");
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, DUI_CTR_MENU) == 0 ) 
        return static_cast<CMenuUI*>(this);

    return CListUI::GetInterface(pstrName);
}

void CMenuUI::DoEvent(TEventUI& event)
{
	return __super::DoEvent(event);
}

bool CMenuUI::Add(CControlUI* pControl)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if (pMenuItem == NULL)
		return false;
	
	CDuiString szLocation;
	if(m_pBirthElement)
	{
		szLocation = m_pBirthElement->GetLocation();
		CDuiString szTmp;
		szTmp.Format(_T("s%d"),GetCount());
		szLocation += szTmp;
	}
	else
		szLocation.Format(_T("%d"),GetCount());
	pMenuItem->SetLocation(szLocation);

	

	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		CMenuElementUI *pSubMenu = (CMenuElementUI *)(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));
		if (pSubMenu)
		{
			CDuiString szSubLocation = pSubMenu->GetLocation();
			CDuiString szCurLocation = szLocation;
			szCurLocation += _T("s");
			szCurLocation += szSubLocation;
			pSubMenu->SetLocation(szCurLocation);

			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(false);
		}
	}

	return CListUI::Add(pControl);
}

bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if (pMenuItem == NULL)
		return false;
	CDuiString szLocation;
	if(m_pBirthElement)
	{
		szLocation = m_pBirthElement->GetLocation();
		CDuiString szTmp;
		szTmp.Format(_T("s%d"),iIndex);
		szLocation += szTmp;
	}
	else
		szLocation.Format(_T("%d"),iIndex);
	pMenuItem->SetLocation(szLocation);
	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		CMenuElementUI *pSubMenu = (CMenuElementUI *)(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));
		if (pSubMenu)
		{
			CDuiString szSubLocation = pSubMenu->GetLocation();
			CDuiString szCurLocation = szLocation;
			szCurLocation += _T("s");
			szCurLocation += szSubLocation;
			pSubMenu->SetLocation(szCurLocation);
			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(false);
		}
	}

	return CListUI::AddAt(pControl, iIndex);
}

int CMenuUI::GetItemIndex(CControlUI* pControl) const
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if (pMenuItem == NULL)
		return -1;

	return __super::GetItemIndex(pControl);
}

bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if (pMenuItem == NULL)
		return false;

	return __super::SetItemIndex(pControl, iIndex);
}

bool CMenuUI::Remove(CControlUI* pControl)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if (pMenuItem == NULL)
		return false;

	return __super::Remove(pControl);
}

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CListUI::SetAttribute(pstrName, pstrValue);
}

SIZE CMenuUI::EstimateNeedSize()
{
	if(m_items.GetSize() == 0)
		return EstimateSize(m_cxyFixed);

	RECT rc = m_rcItem;
	rc.top += m_rcInset.top;
	rc.bottom -= m_rcInset.bottom;

	int cxFixed = 0;
	int cyFixed = 0;
	int nEstimateNum = 0;
	for( int it = 0; it < GetCount(); it++ ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;
		SIZE sz = pControl->EstimateSize(m_cxyFixed);
		if( sz.cx > 0)
		{
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			if(sz.cx > cxFixed)
				cxFixed = sz.cx;
		}
		if(sz.cy > 0)
		{
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			cyFixed += sz.cy;
		}
		cyFixed += pControl->GetPadding().top + pControl->GetPadding().bottom;
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;
	SIZE szAvailable = {-1,-1};
	if(cxFixed > 0)
	{
		szAvailable.cx = cxFixed;
		if(cxFixed < m_cxyFixed.cx)
			szAvailable.cx = m_cxyFixed.cx;
	}
	else
		szAvailable.cx = m_cxyFixed.cx;

	if(cyFixed > 0)
	{
		szAvailable.cy = cyFixed;
		if(cyFixed < m_cxyFixed.cy)
			szAvailable.cy = m_cxyFixed.cy;
	}
	else
		szAvailable.cy = m_cxyFixed.cy;
	return szAvailable;
}

void CMenuUI::SetBirthElement( CMenuElementUI * pControl)
{
	m_pBirthElement = pControl;
}

CMenuElementUI *CMenuUI::GetBirthElement()
{
	return m_pBirthElement;
}

void CMenuUI::SetBelongMenuWnd( CMenuWnd *pBelongWnd )
{
	m_pBelongWnd = pBelongWnd;

	CMenuWnd *pParent = m_pBelongWnd;
	while(pParent)
	{
		if(pParent->m_pLayout)
		{
			CMenuElementUI *pParentElement = pParent->m_pLayout->GetBirthElement();
			CMenuUI *pParentMenu = (CMenuUI *)(pParentElement->GetParentByClass(DUI_CTR_MENU));
			if(pParentMenu)
				pParent = pParentMenu->GetBelongMenuWnd();
			else
				break;
		}
		else
			break;
	}
	for(int i = 0 ; i < GetCount() ; i++)
	{
		CMenuElementUI *pItem = (CMenuElementUI *)GetItemAt(i);
		if(pParent&&pParent->CheckDisableItems(pItem->GetLocation()))
			pItem->SetEnabled(false);
	}
}

CMenuWnd* CMenuUI::GetBelongMenuWnd()
{
	return m_pBelongWnd;
}

void CMenuUI::DoInit()
{
	__super::DoInit();
}

CMenuWnd::CMenuWnd(HWND hParent)
: m_hParent(hParent)
, m_pOwner(NULL)
, m_pLayout()
, m_xml(_T(""))
{
	m_szCurLocation = _T("");
	m_vDisableItems.empty();
	m_vDisableItems.clear();
	m_pNotifier = NULL;
}

CMenuWnd * CMenuWnd::CreateDuiMenu(HWND hWnd)
{
	CMenuWnd *pMenu = new CMenuWnd(hWnd);
	return pMenu;
}


BOOL CMenuWnd::Receive(ContextMenuParam param)
{
	switch(param.wParam)
	{
	case 1:
		Close();
		break;
	case 2:
		{
            HWND hParent = GetParent(m_hWnd);
			while (hParent != NULL)
			{
				if (hParent == param.hWnd)
				{
					Close();
					break;
				}
				hParent = GetParent(hParent);
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}

void CMenuWnd::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_ITEMCLICK) == 0)
    {
		CMenuElementUI *pControl = (CMenuElementUI *)msg.pSender;
		m_szCurLocation = pControl->GetLocation();
    }
}

void CMenuWnd::Init(CMenuElementUI* pOwner, UINT idxml, LPCTSTR pSkinType, POINT point)
{
    STRINGorID strxml(idxml);
    return Init(pOwner,strxml.m_lpstr,pSkinType,point);
}

void CMenuWnd::Init(CMenuElementUI* pOwner, LPCTSTR xml, LPCTSTR pSkinType, POINT point)
{
	m_BasedPoint = point;
    m_pOwner     = pOwner;
    m_pLayout    = NULL;
    m_xml        = xml;

	if (pSkinType != NULL)
		m_sType = pSkinType;

	s_context_menu_observer.AddReceiver(this);

	Create((m_pOwner == NULL) ? m_hParent : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());

    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) 
        hWndParent = ::GetParent(hWndParent);

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
    return _T("MenuWnd");
}

void CMenuWnd::OnFinalMessage(HWND hWnd)
{
	m_pm.RemoveNotifier(this);
	RemoveObserver();
	if( m_pOwner != NULL ) 
    {
		for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
        {
			if( static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)) != NULL ) 
            {
				(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pOwner->GetParent());
				(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetVisible(false);
				(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(false);
			}
		}

		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
	}
	m_vDisableItems.clear();
	m_vDisableItems.empty();


	CMenuWnd *pParent = this;
	while(pParent)
	{
		if(pParent->m_pLayout)
		{
			CMenuElementUI *pParentElement = pParent->m_pLayout->GetBirthElement();
			CMenuUI *pParentMenu = (CMenuUI *)(pParentElement->GetParentByClass(DUI_CTR_MENU));
			if(pParentMenu)
				pParent = pParentMenu->GetBelongMenuWnd();
			else
				break;
		}
		else
			break;
	}
	if(pParent)
		pParent->SetCurLocation(m_szCurLocation);

	if(m_pNotifier)
	{
		TNotifyUI msg;
		msg.sType = DUI_MSGTYPE_MENUCLOSE;
		if(m_szCurLocation != _T(""))
			msg.wParam = (WPARAM)m_szCurLocation.GetData();
		else
			msg.wParam = 0;
		msg.lParam = (LPARAM)this;
		m_pNotifier->Notify(msg);
	}
    delete this;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bHandle = TRUE;
    LRESULT lRes = 0;

    switch(uMsg)
    {
    case WM_CREATE: lRes = OnCreate(uMsg,wParam,lParam,bHandle); break;
    case WM_CLOSE:  lRes = OnClose(uMsg,wParam,lParam,bHandle); break;
    case WM_KEYDOWN: lRes = OnKeyDown(uMsg,wParam,lParam,bHandle); break;
    case WM_KILLFOCUS: lRes = OnKillFocus(uMsg,wParam,lParam,bHandle); break;
    case WM_RBUTTONDOWN: lRes = OnRButtonDown(uMsg,wParam,lParam,bHandle); break;
    case WM_RBUTTONUP: lRes = OnRButtonUp(uMsg,wParam,lParam,bHandle); break;
    case WM_RBUTTONDBLCLK: lRes = OnRButtonDbClick(uMsg,wParam,lParam,bHandle); break;
    case WM_CONTEXTMENU: lRes = OnContextMenu(uMsg,wParam,lParam,bHandle); break;
    default: bHandle = FALSE; break;
    }

    if(bHandle == TRUE) return lRes;
	
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
        return lRes;

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMenuWnd::OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    if( m_pOwner != NULL) 
    {
        // 去掉窗口标题栏
        LONG styleValue = ::GetWindowLongPtr(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLongPtr(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);
        ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
            rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

        m_pm.Init(m_hWnd);
		if(m_pOwner->GetManager()->GetRenderCore()->IsWindowLayered())
			m_pm.SetBackgroundTransparent(true);
        // 创建新的子菜单
        m_pLayout = new CMenuUI();
		m_pLayout->SetBirthElement(m_pOwner);
		
        // 继承拥有者的资源
        m_pm.UseParentResource(m_pOwner->GetManager());
        m_pLayout->SetManager(&m_pm, NULL, true);

        // 获取缺省的菜单样式
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
        if( pDefaultAttributes ) 
            m_pLayout->ApplyAttributeList(pDefaultAttributes);

        // 不允许自动销毁
        m_pLayout->SetAutoDestroy(false);

        for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
        {
            if(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT) != NULL )
            {
                (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pLayout);
                m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
            }
        }
		m_pLayout->SetBelongMenuWnd(this);

        m_pm.AttachDialog(m_pLayout);
        m_pm.AddNotifier(this);

        // 设置菜单显示的位置
        RECT rcOwner = m_pOwner->GetPos();
        RECT rc = rcOwner;

        int cxFixed = 0;
        int cyFixed = 0;

        MONITORINFO oMonitor = {}; 
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;

        SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

        // 计算子菜单的尺寸
        for( int it = 0; it < m_pOwner->GetCount(); it++ ) 
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));

            if(pControl->GetInterface(DUI_CTR_MENUELEMENT) != NULL )
            {
                SIZE sz = pControl->EstimateNeedSize();
                cyFixed += sz.cy;

                if( cxFixed < sz.cx)
                    cxFixed = sz.cx;
            }
        }

        // 上级菜单窗口坐标
        RECT rcWindow;
        GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

        int ItemHeight = rcOwner.bottom - rcOwner.top;

        POINT ptRight;
        ptRight.x = rcWindow.right + MENUWIN_INSET_CAP;
        ptRight.y = rcWindow.top + rcOwner.top;

        POINT ptLeft;
        ptLeft.x = rcWindow.left - MENUWIN_INSET_CAP;
        ptLeft.y = ptRight.y;

        int nX = ptRight.x;
        int nY = ptRight.y;
        RECT rtRect = {nX, nY, nX + cxFixed, nY + cyFixed};

        // 超过右边界
        if(nX + cxFixed > rcWork.right)
        {
            nX = ptLeft.x - cxFixed;
        }

        // 超过下边界
        if(nY + cyFixed > rcWork.bottom)
        {
            nY = ptRight.y + ItemHeight - cyFixed;
        }

        MoveWindow(m_hWnd, nX, nY, cxFixed, cyFixed, FALSE);
    }
    else 
    {
		m_pm.Init(m_hWnd);

		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(STRINGorID(m_xml), m_sType.GetData(), NULL, &m_pm);

		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		CMenuUI *pMenu = (CMenuUI *)(pRoot->GetInterface(DUI_CTR_MENU));
		if(pMenu)
		{
			pMenu->SetBirthElement(NULL);
			pMenu->SetBelongMenuWnd(this);
		}

		// 获取显示器有效显示区域
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;

		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
		szAvailable = pRoot->EstimateNeedSize();
		m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

		DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;

		SIZE szInit = m_pm.GetInitSize();
		CDuiPoint point = m_BasedPoint;
		CDuiRect rc;
        
        rc.left   = point.x;
        rc.top    = point.y;
        rc.right  = rc.left + szInit.cx;
        rc.bottom = rc.top + szInit.cy;

        // 根据菜单的大小,判断菜单显示的方向
        if(rc.right >= rcWork.right)
            dwAlignment |= eMenuAlignment_Right;
        else
            dwAlignment |= eMenuAlignment_Left;

        if(rc.bottom >= rcWork.bottom)
            dwAlignment |= eMenuAlignment_Bottom;
        else
            dwAlignment |= eMenuAlignment_Top;

        int nWidth = rc.GetWidth();
        int nHeight = rc.GetHeight();

        if (dwAlignment & eMenuAlignment_Right)
        {
            rc.right = point.x;
            rc.left = rc.right - nWidth;
        }

        if (dwAlignment & eMenuAlignment_Bottom)
        {
            rc.bottom = point.y;
            rc.top = rc.bottom - nHeight;
        }

        // 调整菜单显示位置
        SetForegroundWindow(m_hWnd);
        MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
        SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_SHOWWINDOW);
    }
    return 0;
}

LRESULT CMenuWnd::OnClose(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    bHandle = FALSE;

    if( m_pOwner != NULL )
    {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos());
        m_pOwner->SetFocus();
    }

    return 0;
}

LRESULT CMenuWnd::OnKeyDown(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    bHandle = FALSE;

    if( wParam == VK_ESCAPE)
    {
        Close();
    }

    return 0;
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    bHandle = FALSE;
    BOOL bInMenuWindowList = FALSE;

    HWND hFocusWnd = (HWND)wParam;
    
    ContextMenuParam param;
    param.hWnd = GetHWND();

    ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
    ReceiverImplBase<BOOL, ContextMenuParam>* pReceiver = iterator.next();
    while( pReceiver != NULL ) 
    {
        CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
        if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) 
        {
            bInMenuWindowList = TRUE;
            break;
        }

        pReceiver = iterator.next();
    }

    if( !bInMenuWindowList ) 
    {
        param.wParam = 1;
        s_context_menu_observer.RBroadcast(param);

        bHandle = TRUE;
        return 0;
    }

    return 0;
}

LRESULT CMenuWnd::OnRButtonDown(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    return 0;
}

LRESULT CMenuWnd::OnRButtonUp(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    return 0;
}

LRESULT CMenuWnd::OnRButtonDbClick(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    return 0;
}

LRESULT CMenuWnd::OnContextMenu(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle)
{
    return 0;
}

void CMenuWnd::SetNotifier( INotifyUI *pNotifier )
{
	m_pNotifier = pNotifier;
}

INotifyUI * CMenuWnd::GetNotifier()
{
	return m_pNotifier;
}

void CMenuWnd::AddDisableItmes(char* strItems)
{
	string szItem(strItems);
	m_vDisableItems.push_back(szItem);
}

bool CMenuWnd::CheckDisableItems(LPCTSTR szItemLocation)
{
	for(int i = 0 ; i < m_vDisableItems.size(); i++)
	{
		CString szDisableItem(m_vDisableItems[i].c_str());
		if(szDisableItem == szItemLocation)
			return true;
	}
	return false;
}

void CMenuWnd::SetCurLocation( CDuiString szLocation )
{
	m_szCurLocation = szLocation;
}

CMenuElementUI::CMenuElementUI():
m_pWindow(NULL)
{
	m_cxyFixed.cy = 25;
	m_bMouseChildEnabled = true;

	SetMouseChildEnabled(false);
	m_szLocation = _T("");
}

CMenuElementUI::~CMenuElementUI()
{}

LPCTSTR CMenuElementUI::GetClass() const
{
	return _T("MenuElementUI");
}

LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, DUI_CTR_MENUELEMENT) == 0 ) return static_cast<CMenuElementUI*>(this);    
    return CListContainerElementUI::GetInterface(pstrName);
}

void CMenuElementUI::DoPaint( const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	CMenuElementUI::DrawItemBk(m_rcItem);
	DrawItemText(m_rcItem);
	DrawIcon();
	for (int i = 0; i < GetCount(); ++i)
	{
		if (GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT) == NULL)
			GetItemAt(i)->DoPaint(rcPaint);
	}
}

void CMenuElementUI::DrawItemText( const RECT& rcItem)
{
    if( m_sText.IsEmpty() ) return;

    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    DWORD iTextColor = pInfo->dwTextColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += pInfo->rcTextPadding.left;
    rcText.right -= pInfo->rcTextPadding.right;
    rcText.top += pInfo->rcTextPadding.top;
    rcText.bottom -= pInfo->rcTextPadding.bottom;

    if( pInfo->bShowHtml )
		m_pManager->GetRenderCore()->DrawHtmlText(m_pManager, rcText, m_sText, iTextColor, \
		NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle,m_bTextGlow);
    else
        m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, m_sText, iTextColor, \
        pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle,m_bTextGlow);
}


void CMenuElementUI::DoEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_MOUSEENTER)
	{
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT) != NULL )
			{
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetVisible(true);
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			m_pOwner->SelectItem(GetIndex(), true);
			CreateMenuWnd();
		}
		else
		{
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 2;
			s_context_menu_observer.RBroadcast(param);
		}
	}
	else if(event.Type == UIEVENT_BUTTONDOWN)
	{
		if( m_pWindow ) return;
		if(!IsEnabled()) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT) != NULL )
			{
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetVisible(true);
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			CreateMenuWnd();
		}
		else
		{
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 1;
			s_context_menu_observer.RBroadcast(param);
		}
	}
    CListContainerElementUI::DoEvent(event);
}

bool CMenuElementUI::Activate()
{
	if (CListContainerElementUI::Activate() && m_bSelected)
	{
		if( m_pWindow ) return true;
		bool hasSubMenu = false;
		for (int i = 0; i < GetCount(); ++i)
		{
			if (GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT) != NULL)
			{
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetVisible(true);
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT)))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if (hasSubMenu)
		{
			CreateMenuWnd();
		}
		else
		{
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 1;
			s_context_menu_observer.RBroadcast(param);
		}

		return true;
	}
	return false;
}

CMenuWnd* CMenuElementUI::GetMenuWnd()
{
	return m_pWindow;
}

void CMenuElementUI::CreateMenuWnd()
{
	if( m_pWindow ) return;

	m_pWindow = new CMenuWnd(m_pManager->GetPaintWindow());
	
	ASSERT(m_pWindow);

	ContextMenuParam param;
	param.hWnd = m_pManager->GetPaintWindow();
	param.wParam = 2;
	s_context_menu_observer.RBroadcast(param);

	m_pWindow->Init(static_cast<CMenuElementUI*>(this), _T(""), _T(""), CDuiPoint());
}

UiLib::CDuiString CMenuElementUI::GetLocation()
{
	return m_szLocation;
}

void CMenuElementUI::SetLocation( CDuiString szLocation )
{
	m_szLocation = szLocation;
}

bool CMenuElementUI::Add( CControlUI* pControl )
{
	bool bRes =  __super::Add(pControl);
	CMenuElementUI *pSubMenu = (CMenuElementUI *)(pControl->GetInterface(DUI_CTR_MENUELEMENT));
	if(pSubMenu)
	{
		int nIndex = GetCount()-1;
		CDuiString szLocation;
		szLocation.Format(_T("%d"),nIndex);
		pSubMenu->SetLocation(szLocation);
	}
	return bRes;
}

void CMenuElementUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if( _tcscmp(pstrName, _T("iconormalimage")) == 0 ) SetNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("icohotimage")) == 0 ) SetHotImage(pstrValue);
	__super::SetAttribute(pstrName,pstrValue);
}

void CMenuElementUI::SetNormalImage( LPCTSTR pStrImage )
{
	m_sNormalImage = pStrImage;
	Invalidate();
}

void CMenuElementUI::SetHotImage( LPCTSTR pStrImage )
{
	m_sHotImage = pStrImage;
	Invalidate();
}

void CMenuElementUI::DrawIcon()
{
	if( (m_uButtonState & UISTATE_HOT) != 0 ) 
	{
		if( !m_sHotImage.IsEmpty() ) 
		{
			if( !DrawImage(m_sHotImage) )
				m_sHotImage.Empty();
			return;
		}
	}

	if( !m_sNormalImage.IsEmpty() ) 
	{
		if( !DrawImage(m_sNormalImage) ) 
			m_sNormalImage.Empty();
	}
}



} // namespace DuiLib
