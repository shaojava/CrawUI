#include "StdAfx.h"
#include "regex"

namespace UiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//
REGIST_DUICLASS(CComboBoxUI);

class CComboBoxEditWnd : public CWindowWnd
{
public:
	CComboBoxEditWnd();

	void Init(CComboBoxUI* pOwner,BOOL bPop);
	RECT CalPos();
	LPTSTR GetWindowText();
	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	CComboBoxUI* m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;
};

CComboBoxEditWnd::CComboBoxEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
{
}

void CComboBoxEditWnd::Init(CComboBoxUI* pOwner,BOOL bPop)
{
	if(!pOwner) return;
	m_pOwner = pOwner;
	RECT rcPos = CalPos();
	UINT uStyle;
	if(bPop)
	{
		uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE | ES_MULTILINE;
		RECT rcWnd={0};
		::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
		int nOffsexFix = 0;
		RenderWay uWay = m_pOwner->GetManager()->GetRenderCore()->GetRenderType();
		if(uWay == D2D_RENDER)
			nOffsexFix = 0;
		else
			nOffsexFix = 2;
		rcPos.left += rcWnd.left+nOffsexFix;
		rcPos.right += rcWnd.left+nOffsexFix;
		rcPos.top += rcWnd.top;
		rcPos.bottom += rcWnd.top;
	}
	else
		uStyle = WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE;

	Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

	HFONT hFont=NULL;
	int iFontIndex=m_pOwner->GetFont();
	if (iFontIndex!=-1)
		hFont=m_pOwner->GetManager()->GetFont(iFontIndex)->hFont;
	if (hFont==NULL)
		hFont=m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;
	SetWindowFont(m_hWnd, hFont, TRUE);

	Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
	Edit_SetText(m_hWnd, m_pOwner->GetText());
	Edit_SetModify(m_hWnd, FALSE);
	SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	Edit_SetSel(m_hWnd, 0,-1);
	::SetFocus(m_hWnd);
	m_bInit = true;
}

RECT CComboBoxEditWnd::CalPos()
{
	CDuiRect rcPos = m_pOwner->GetPos();
	RECT rcInset = m_pOwner->GetEditInset();
	rcPos.left += rcInset.left+1;
	rcPos.top += rcInset.top+1;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	return rcPos;
}

LPCTSTR CComboBoxEditWnd::GetWindowClassName() const
{
	return _T("EditWnd");
}

LPTSTR CComboBoxEditWnd::GetWindowText()
{
	if(m_hWnd)
	{
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		return pstr;
	}
	return NULL;
}

LPCTSTR CComboBoxEditWnd::GetSuperClassName() const
{
	return WC_EDIT;
}

void CComboBoxEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
	m_pOwner->Invalidate();
	// Clear reference and die
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pEditWnd = NULL;
	delete this;
}

LRESULT CComboBoxEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	if( uMsg == OCM_COMMAND ) {
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if( uMsg == WM_KEYDOWN ) {
		TEventUI event = { 0 };
		event.Type = UIEVENT_KEYDOWN;
		event.chKey = (TCHAR)wParam;
		event.wKeyState = MapKeyState();
		event.dwTimestamp = ::GetTickCount();
		m_pOwner->Event(event);

		if (TCHAR(wParam) == VK_RETURN )
		{
			m_pOwner->GetManager()->HandleIDOKEvent();
			m_pOwner->GetManager()->SendNotify(m_pOwner, _T("editreturn"));
		}
	}
	else if( uMsg == WM_KILLFOCUS )
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		m_pOwner->m_pEditWnd = NULL;
		PostMessage(WM_CLOSE);
		return lRes;
	}
	else if(uMsg == WM_CHAR)
	{
		if(m_pOwner && !(GetKeyState(VK_CONTROL) & 0x8000))
		{
			CDuiString szRegex = m_pOwner->GetRegex();
			if(!szRegex.IsEmpty() && wParam != 8)
			{
				int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
				int nChar = WideCharToMultiByte(codepage, 0, szRegex, -1, 0, 0, 0, 0);
				int nSize = nChar * sizeof(char) + 1;
				char* pchBuffer = new char[nSize];
				nChar = WideCharToMultiByte(codepage, 0, szRegex, -1, pchBuffer, nSize, 0, 0);
				if( nChar == 0 )
				{
					delete[] pchBuffer;
					return NULL;
				}
				pchBuffer[nChar]= 0;
				std::string szRes(pchBuffer);
				delete pchBuffer;
				pchBuffer = NULL;
	
				const std::regex pattern(szRes.c_str());
				TCHAR * szMatch = new TCHAR[2];
				szMatch[0] = wParam;
				szMatch[1] = 0;
				nChar = WideCharToMultiByte(codepage, 0, szMatch, -1, 0, 0, 0, 0);
				nSize = nChar * sizeof(char) + 1;
				pchBuffer = new char[nSize];
				nChar = WideCharToMultiByte(codepage, 0, szMatch, -1, pchBuffer, nSize, 0, 0);
				pchBuffer[nChar] = 0;
				SAFE_DELETE(szMatch);
				bool match = std::regex_match(pchBuffer, pattern);
				SAFE_DELETE(pchBuffer);
				if(!match)
					return lRes;
			}
		}
		TEventUI event = { 0 };
		event.Type = UIEVENT_CHAR;
		event.chKey = (TCHAR)wParam;
		event.wKeyState = MapKeyState();
		event.dwTimestamp = ::GetTickCount();
		event.pSender = m_pOwner;
		m_pOwner->Event(event);
		if(event.bHandle)
			return lRes;
		bHandled = FALSE;
	}
	else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = m_pOwner->GetTextColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		if( m_hBkBrush == NULL ) 
			return (LRESULT)::HBRUSH(GetStockObject(WHITE_BRUSH));
		return (LRESULT)m_hBkBrush;
	}
	else bHandled = FALSE;
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CComboBoxEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( !m_bInit ) return 0;
	if( m_pOwner == NULL ) return 0;
	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	ASSERT(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_sText = pstr;
	TEventUI event = { 0 };
	event.Type = UIEVENT_TEXTCHANGE;
	event.pSender = m_pOwner;
	event.dwTimestamp = ::GetTickCount();
	m_pOwner->Event(event);
	m_pOwner->GetManager()->SendNotify(m_pOwner, _T("textchanged"));
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////
////

class CComboBoxWnd : public CWindowWnd
{
public:
    void Init(CComboBoxUI* pOwner);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);
	void FindBegin(int nLastPos,int nScrollRange);
#if(_WIN32_WINNT >= 0x0501)
	virtual UINT GetClassStyle() const;
#endif

public:
    CPaintManagerUI m_pm;
    CComboBoxUI* m_pOwner;
    CVerticalLayoutUI* m_pLayout;
    int m_iOldSel;
};


void CComboBoxWnd::Init(CComboBoxUI* pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
    for( int it = 0; it < pOwner->GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }

    Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	m_pOwner->SetSelAll();
}

LPCTSTR CComboBoxWnd::GetWindowClassName() const
{
    return _T("ComboWnd");
}

void CComboBoxWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pDropWnd = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
	m_pOwner->SetSelAll();
	m_pOwner->SelectEnd();
    delete this;
}

LRESULT CComboBoxWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_CREATE ) {
        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CVerticalLayoutUI;
        m_pm.UseParentResource(m_pOwner->GetManager());
        m_pLayout->SetManager(&m_pm, NULL, true);
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
        if( pDefaultAttributes ) {
            m_pLayout->ApplyAttributeList(pDefaultAttributes);
        }
        m_pLayout->SetInset(CDuiRect(1, 1, 1, 1));
        m_pLayout->SetBkColor(0xFFFFFFFF);
        m_pLayout->SetBorderColor(0xFFC6C7D2);
        m_pLayout->SetBorderSize(1);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
        for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
          m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
        m_pm.AttachDialog(m_pLayout);
        
        return 0;
    }
    else if( uMsg == WM_CLOSE ) {
		int nYRange = (m_pLayout->GetScrollRange()).cy;
		m_pOwner->SetLastScrollPos(m_pLayout->GetScrollPos().cy,nYRange);
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos());
        
    }
    else if( uMsg == WM_LBUTTONUP ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
        CControlUI* pControl = m_pm.FindControl(pt);
        if( pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 ) PostMessage(WM_KILLFOCUS);
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel, true);
            EnsureVisible(m_iOldSel);
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
			EnsureVisible(m_pOwner->GetCurSel());
            m_pOwner->DoEvent(event);
            return 0;
        }
    }
    else if( uMsg == WM_MOUSEWHEEL ) {
		if(m_pLayout)
		{
			int zDelta = (int) (short) HIWORD(wParam);
			TEventUI event = { 0 };
			event.Type = UIEVENT_SCROLLWHEEL;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.dwTimestamp = ::GetTickCount();
			m_pLayout->DoEvent(event);
		}
        return 0;
    }
    else if( uMsg == WM_KILLFOCUS ) {
        if( m_hWnd != (HWND) wParam ) 
		{
			PostMessage(WM_CLOSE);
		}
    }

    LRESULT lRes = 0;
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CComboBoxWnd::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
    CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    int iPos = m_pLayout->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
}

void CComboBoxWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}
void CComboBoxWnd::FindBegin(int nLastPos,int nScrollRange)
{
	m_pLayout->SetScrollRange(CDuiSize(0,nScrollRange));
	m_pLayout->SetScrollPos(CDuiSize(0,nLastPos));
}

#if(_WIN32_WINNT >= 0x0501)
UINT CComboBoxWnd::GetClassStyle() const
{
	return __super::GetClassStyle() | CS_DROPSHADOW;
}
#endif
////////////////////////////////////////////////////////


CComboBoxUI::CComboBoxUI() : m_pDropWnd(NULL), m_pEditWnd(NULL),m_iCurSel(-1), m_uButtonState(0),m_uTextStyle(DT_VCENTER)
{
	m_bNeedTab = true;
	m_bSelectChange = false;
	m_iCurSel = -1;
	m_iOldSel = -1;
	m_uMaxChar = -1;
	m_rcEditInset.left = m_rcEditInset.right = m_rcEditInset.top = m_rcEditInset.bottom = 0;
	m_iFont = -1;
	m_dwTextColor = 0;
	m_bWheelReaction = true;
	m_nArrowWidth = 22;
	m_nLastPos = 0;
	m_nScrollRange = 100;
    m_szDropBox = CDuiSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

    m_ListInfo.nColumns = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER;
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.dwLineColor = 0;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

CComboBoxUI::~CComboBoxUI()
{
	if(m_pDropWnd)
		m_pDropWnd->SendMessage(WM_CLOSE);
	if(m_pEditWnd)
		m_pEditWnd->SendMessage(WM_CLOSE);
}

LPCTSTR CComboBoxUI::GetClass() const
{
    return _T("ComboUI");
}

LPVOID CComboBoxUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ComboBox")) == 0 ) return static_cast<CComboBoxUI*>(this);
    if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

UINT CComboBoxUI::GetControlFlags() const
{
    return IsWantTab();
}

void CComboBoxUI::DoInit()
{
	__super::DoInit();
}

void CComboBoxUI::SetLastScrollPos(int nPos,int nRange)
{
	m_nLastPos = nPos;
	m_nScrollRange = nRange;
}
int CComboBoxUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CComboBoxUI::SelectItem(int iIndex, bool bTakeFocus)
{
    if( m_pDropWnd != NULL ) m_pDropWnd->Close();
    if( iIndex == m_iCurSel ) return true;
	m_bSelectChange = true;
	m_iOldSel = m_iCurSel;
    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        if( !pControl ) return false;
        IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) pListItem->Select(false);
        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;
    if( m_items.GetSize() == 0 ) return false;
    if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
    CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
    if( !pControl || !pControl->IsEnabled() ) return false;
	m_sText = pControl->GetText();
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem == NULL ) return false;
    m_iCurSel = iIndex;
    if( m_pDropWnd != NULL || bTakeFocus ) pControl->SetFocus();
    pListItem->Select(true);
    Invalidate();
    return true;
}

bool CComboBoxUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
    if( !CContainerUI::SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CComboBoxUI::Add(CControlUI* pControl)
{
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem != NULL ) 
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }
    return CContainerUI::Add(pControl);
}

bool CComboBoxUI::Add(CDuiString strtext)
{
	m_VectorFont.push_back(strtext);
	return true;
}

void CComboBoxUI::Sort(bool bSort)
{
	if(bSort)
		sort(m_VectorFont.begin(), m_VectorFont.end(),greater<CDuiString>());
	OnLoadStringList();
}

void CComboBoxUI::OnLoadStringList()
{
	for(int i = 0; i < (int)(m_VectorFont.size()); i++)
	{
		CListLabelElementUI *pListItem = new CListLabelElementUI;
		pListItem->SetText(m_VectorFont[i]);
		pListItem->SetOwner(this);
		Add(pListItem);
	}
}

bool CComboBoxUI::AddAt(CControlUI* pControl, int iIndex)
{
    if (!CContainerUI::AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool CComboBoxUI::Remove(CControlUI* pControl)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

bool CComboBoxUI::RemoveAt(int iIndex)
{
    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

void CComboBoxUI::RemoveAll(bool bUpdate)
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll(bUpdate);
	m_VectorFont.clear();
}

void CComboBoxUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
		if( IsEnabled() )
			ActivateEditWnd();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
		if(m_pDropWnd)
			m_pDropWnd->PostMessage(WM_CLOSE);
		if(m_pEditWnd)
			m_pEditWnd->PostMessage(WM_CLOSE);
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN )
    {
        if( IsEnabled() ) {
            RECT rcEdit = m_rcItem;
			rcEdit.left += m_rcEditInset.left;
			rcEdit.right -= m_rcEditInset.right;
			rcEdit.top += m_rcEditInset.top;
			rcEdit.bottom -= m_rcEditInset.bottom;
			 m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			if(PtInRect(&rcEdit,event.ptMouse))
				ActivateEditWnd();
			else
			{
				RECT rcArrow = m_rcItem;
				rcArrow.left = rcArrow.right-m_nArrowWidth;
				if(PtInRect(&rcArrow,event.ptMouse))
					ActivateDropWnd();
			}
           
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        switch( event.chKey ) {
        case VK_F4:
            ActivateDropWnd();
            return;
        case VK_UP:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_DOWN:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_PRIOR:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_NEXT:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_HOME:
            SelectItem(FindSelectable(0, false));
            return;
        case VK_END:
            SelectItem(FindSelectable(GetCount() - 1, true));
            return;
        }
    }
    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
		if(m_bWheelReaction)
		{
			bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
			SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
		}
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( (m_uButtonState & UISTATE_HOT) == 0  )
                m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( (m_uButtonState & UISTATE_HOT) != 0 ) {
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    CControlUI::DoEvent(event);
}

SIZE CComboBoxUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 12);
    return CControlUI::EstimateSize(szAvailable);
}

bool CComboBoxUI::ActivateEditWnd()
{
	if( !CControlUI::Activate() || !m_pManager ) return false;
	if( m_pEditWnd ) return true;
	m_pEditWnd = new CComboBoxEditWnd();
	ASSERT(m_pEditWnd);
	m_pEditWnd->Init(this,m_pManager->GetRenderCore()->IsWindowLayered());
	Invalidate();
	return true;
}

bool CComboBoxUI::ActivateDropWnd()
{
    if( !CControlUI::Activate() ) return false;
    if( m_pDropWnd ) 
	{
		m_pDropWnd->SendMessage(WM_CLOSE);
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
			m_uButtonState &= ~UISTATE_CAPTURED;
		if( (m_uButtonState & UISTATE_PUSHED) != 0 ) 
			m_uButtonState &= ~UISTATE_PUSHED;
		Invalidate();
		return true;
	}
    m_pDropWnd = new CComboBoxWnd();
    ASSERT(m_pDropWnd);
    m_pDropWnd->Init(this);
	m_pDropWnd->FindBegin(m_nLastPos,m_nScrollRange);
    if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("dropdown"));
    Invalidate();
    return true;
}

CDuiString CComboBoxUI::GetText() const
{
    return CControlUI::GetText();
}

void CComboBoxUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);
    if( !IsEnabled() ) m_uButtonState = 0;
}

CDuiString CComboBoxUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes;
}

void CComboBoxUI::SetDropBoxAttributeList(LPCTSTR pstrList)
{
    m_sDropBoxAttributes = pstrList;
}

SIZE CComboBoxUI::GetDropBoxSize() const
{
    return m_szDropBox;
}

void CComboBoxUI::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

RECT CComboBoxUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CComboBoxUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

LPCTSTR CComboBoxUI::GetNormalImage() const
{
    return m_sNormalImage;
}

void CComboBoxUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboBoxUI::GetHotImage() const
{
    return m_sHotImage;
}

void CComboBoxUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboBoxUI::GetPushedImage() const
{
    return m_sPushedImage;
}

void CComboBoxUI::SetPushedImage(LPCTSTR pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboBoxUI::GetFocusedImage() const
{
    return m_sFocusedImage;
}

void CComboBoxUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboBoxUI::GetDisabledImage() const
{
    return m_sDisabledImage;
}

void CComboBoxUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

TListInfoUI* CComboBoxUI::GetListInfo()
{
    return &m_ListInfo;
}

void CComboBoxUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    Invalidate();
}

void CComboBoxUI::SetItemTextStyle(UINT uStyle)
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

RECT CComboBoxUI::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void CComboBoxUI::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    Invalidate();
}

void CComboBoxUI::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void CComboBoxUI::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
}

void CComboBoxUI::SetItemBkImage(LPCTSTR pStrImage)
{
    m_ListInfo.sBkImage = pStrImage;
}

DWORD CComboBoxUI::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD CComboBoxUI::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR CComboBoxUI::GetItemBkImage() const
{
	return m_ListInfo.sBkImage;
}

bool CComboBoxUI::IsAlternateBk() const
{
    return m_ListInfo.bAlternateBk;
}

void CComboBoxUI::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
}

void CComboBoxUI::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
}

void CComboBoxUI::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
}

void CComboBoxUI::SetSelectedItemImage(LPCTSTR pStrImage)
{
	m_ListInfo.sSelectedImage = pStrImage;
}

DWORD CComboBoxUI::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD CComboBoxUI::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR CComboBoxUI::GetSelectedItemImage() const
{
	return m_ListInfo.sSelectedImage;
}

void CComboBoxUI::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
}

void CComboBoxUI::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
}

void CComboBoxUI::SetHotItemImage(LPCTSTR pStrImage)
{
    m_ListInfo.sHotImage = pStrImage;
}

DWORD CComboBoxUI::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD CComboBoxUI::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR CComboBoxUI::GetHotItemImage() const
{
	return m_ListInfo.sHotImage;
}

void CComboBoxUI::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
}

void CComboBoxUI::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
}

void CComboBoxUI::SetDisabledItemImage(LPCTSTR pStrImage)
{
    m_ListInfo.sDisabledImage = pStrImage;
}

DWORD CComboBoxUI::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD CComboBoxUI::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR CComboBoxUI::GetDisabledItemImage() const
{
	return m_ListInfo.sDisabledImage;
}

DWORD CComboBoxUI::GetItemLineColor() const
{
	return m_ListInfo.dwLineColor;
}

void CComboBoxUI::SetItemLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwLineColor = dwLineColor;
}

bool CComboBoxUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CComboBoxUI::SetItemShowHtml(bool bShowHtml)
{
    if( m_ListInfo.bShowHtml == bShowHtml ) return;

    m_ListInfo.bShowHtml = bShowHtml;
    Invalidate();
}

void CComboBoxUI::SetArrowWidth(int nArrowWidth)
{
	m_nArrowWidth = nArrowWidth;
}
void CComboBoxUI::SetPos(RECT rc)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
    // Position this control
    CControlUI::SetPos(rc);
}

void CComboBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("editinset")) == 0 ) {
		LPTSTR pstr = NULL;
		m_rcEditInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		m_rcEditInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		m_rcEditInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		m_rcEditInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
	}
	else if (_tcscmp(pstrName, _T("tipimage")) == 0)
		m_sTipImage = pstrValue;
	else if (_tcscmp(pstrName, _T("arrownormal")) == 0)
		m_sArrowNormal = pstrValue;
	else if (_tcscmp(pstrName, _T("arrowhot")) == 0)
		m_sArrowHot = pstrValue;
	else if (_tcscmp(pstrName, _T("arrowpushed")) == 0)
		m_sArrowPushed = pstrValue;
    else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetTextPadding(rcTextPadding);
    }
	else if( _tcscmp(pstrName, _T("selectedid")) == 0 ) 
		SelectItem(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("editmaxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
	else if( _tcscmp(pstrName, _T("editregex")) == 0 )
		m_szEditRegex = pstrValue;
	else if( _tcscmp(pstrName, _T("dropboxsize")) == 0)
	{
		SIZE szDropBoxSize = { 0 };
		LPTSTR pstr = NULL;
		szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		SetDropBoxSize(szDropBoxSize);
	}
    else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetItemTextPadding(rcTextPadding);
    }
    else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemBkColor(clrColor);
    }
	else if( _tcscmp(pstrName, _T("arrowwidth")) == 0 ) SetArrowWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemLineColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetTextColor(clrColor);
	}
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CComboBoxUI::DoPaint(const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) 
	{
		CRenderClip roundClip;
		roundClip.SetPaintManager(m_pManager);
		CRenderClip::GenerateRoundClip(m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
		PaintBkColor();
		PaintBkImage();
		PaintStatusImage();
		PaintText();
		PaintArrow();
		PaintBorder();
	}
	else 
	{
		PaintBkColor();
		PaintBkImage();
		PaintStatusImage();
		PaintText();
		PaintArrow();
		PaintBorder();
	}
}

void CComboBoxUI::PaintStatusImage()
{
    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
        if( !m_sDisabledImage.IsEmpty() ) {
            if( !DrawImage(m_sDisabledImage) ) m_sDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
        if( !m_sPushedImage.IsEmpty() ) {
            if( !DrawImage(m_sPushedImage) ) m_sPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if( !m_sHotImage.IsEmpty() ) {
            if( !DrawImage(m_sHotImage) ) m_sHotImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
        if( !m_sFocusedImage.IsEmpty() ) {
            if( !DrawImage(m_sFocusedImage) ) m_sFocusedImage.Empty();
            else return;
        }
    }

    if( !m_sNormalImage.IsEmpty() ) {
        if( !DrawImage(m_sNormalImage) ) m_sNormalImage.Empty();
        else return;
    }
}

void CComboBoxUI::PaintText()
{
    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left+m_rcEditInset.left;
    rcText.right -= m_rcTextPadding.right+m_rcEditInset.right;
    rcText.top += m_rcTextPadding.top+m_rcEditInset.top;
    rcText.bottom -= m_rcTextPadding.bottom+m_rcEditInset.bottom;
	rcText.right -= m_nArrowWidth;
	m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, m_sText, m_dwTextColor, m_iFont, m_uTextStyle,m_bTextGlow);
}

void CComboBoxUI::SetWheelAble( bool bAble)
{
	m_bWheelReaction = bAble;
}

bool CComboBoxUI::GetHweelAble()
{
	return m_bWheelReaction;
}

void CComboBoxUI::PaintArrow()
{
	if( !m_sTipImage.IsEmpty())
	{
		CDuiRect rcTipSize(m_sTipImage.GetSource());
		CDuiRect rcTipImg(0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
		rcTipImg.right = rcTipImg.left + rcTipSize.GetWidth();
		if( !DrawImage(m_sTipImage,rcTipImg) ) m_sTipImage.Empty();
	}

	CDuiRect rcBmpPart(m_sArrowNormal.GetSource());
	m_nArrowWidth = rcBmpPart.GetWidth();
	CDuiRect rcDest(0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
	rcDest.Deflate(GetBorderSize(), GetBorderSize());
	rcDest.left = rcDest.right - m_nArrowWidth;

	if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		if( !m_sArrowPushed.IsEmpty() ) {
			if( !DrawImage(m_sArrowPushed,rcDest) ) m_sArrowPushed.Empty();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !m_sArrowHot.IsEmpty() ) {
			if( !DrawImage(m_sArrowHot,rcDest) ) m_sArrowHot.Empty();
			else return;
		}
	}
	if( !m_sArrowNormal.IsEmpty() ) {
		if( !DrawImage(m_sArrowNormal,rcDest) ) m_sArrowNormal.Empty();
		else return;
	}
}

int CComboBoxUI::GetFont() const
{
	return m_iFont;
}

void CComboBoxUI::SetFont( int index )
{
	m_iFont = index;
	Invalidate();
}

void CComboBoxUI::SetTextColor( DWORD dwTextColor )
{
	m_dwTextColor = dwTextColor;
	Invalidate();
}

DWORD CComboBoxUI::GetTextColor() const
{
	return m_dwTextColor;
}

RECT CComboBoxUI::GetEditInset()
{
	return m_rcEditInset;
}

void CComboBoxUI::SetTextStyle( UINT uStyle )
{
	m_uTextStyle = uStyle;
	Invalidate();
}

UINT CComboBoxUI::GetTextStyle() const
{
	return m_uTextStyle;
}

void CComboBoxUI::SetSelAll()
{
	if(m_pEditWnd)
	{
		::SetFocus(*m_pEditWnd);
		Edit_SetSel(*m_pEditWnd, 0,-1);
	}
	else
		ActivateEditWnd();
}

void CComboBoxUI::SetMaxChar( int uMax )
{
	m_uMaxChar = uMax;
	if( m_pEditWnd != NULL ) Edit_LimitText(*m_pEditWnd, m_uMaxChar);
}

int CComboBoxUI::GetMaxChar()
{
	return m_uMaxChar;
}

UiLib::CDuiString CComboBoxUI::GetRegex()
{
	return m_szEditRegex;
}

void CComboBoxUI::SelectEnd()
{
	if( m_pManager&&m_bSelectChange) m_pManager->SendNotify(this, _T("itemselect"), m_iCurSel, m_iOldSel);
}

} // namespace UiLib
